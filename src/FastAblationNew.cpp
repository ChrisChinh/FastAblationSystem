// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "StageControl.h"
#include "LaserControl.h"
#include "GalvoControl.h"
#include "../MetalMesh.h"
#include <wchar.h>
#include <string>
#include "KeyboardControl.h"
#include "ConfocalWrapper.h"


using namespace std;

void test();

// Controllers
LaserControl laser = LaserControl();
GalvoControl galvo = GalvoControl();
StageControl stage = StageControl();
ConfocalWrapper confocal = ConfocalWrapper(stage);

// Experiments
MetalMesh mesh = MetalMesh(stage, galvo, laser, confocal);



enum string_code {
	HELP,
	QUIT,
	EXPERIMENT,
	KEYBOARD,
	INVALID,
	TEST,
	LASER,
	CONFOCAL,
	LASER_PREP
};

string_code hash_string(string const& inString) {
	if (inString == "h") return HELP;
	if (inString == "q") return QUIT;
	if (inString == "e") return EXPERIMENT;
	if (inString == "k") return KEYBOARD;
	if (inString == "t") return TEST;
	if (inString == "l") return LASER;
	if (inString == "c") return CONFOCAL;
	if (inString == "lp") return LASER_PREP;
	return INVALID;
}

int main() {
	galvo.openShutter();
	laser.setVoltage(0.0);
	laser.closeGate();
	while (true) {
		cout << "Command (h for help): ";
		string command = "";
		cin >> command;
		switch (hash_string(command)) {
		case HELP:
			cout << "h: Help" << endl;
			cout << "q: Quit" << endl;
			cout << "e: Experiment (run an experiment)" << endl;
			cout << "k: Keyboard (run keyboard control)" << endl;
			cout << "t: Test (run test code)" << endl;
			cout << "l: Laser (set laser voltage)" << endl;
			cout << "lp: Laser prep" << endl;
			break;

		case QUIT:
			return 0;

		case EXPERIMENT:
			cout << "Enter which experiment to run: " << endl;
			cout << "1: Metal mesh" << endl;
			cout << "b: Back" << endl;
			cout << ">> ";
			char exp;
			cin >> exp;
			switch (exp) {
			case '1':
				laser.setVoltage(0.6);
				laser.openGate();
				mesh.setParameter("voltage", 1.05);
				cout << "enter the kerf: ";
				double kerf;
				cin >> kerf;
				mesh.setParameter("kerf", kerf);
				mesh.setParameter("square_length", 37.77);
				mesh.setParameter("square_width", 10.56);
				mesh.setParameter("num_units", 6);
				mesh.setParameter("crosses", 1);
				mesh.setParameter("period", 56.199);
				mesh.setParameter("aperture", 1);
				mesh.setParameter("circle_aperture", 0);
				mesh.setParameter("use_confocal", 0);
				mesh.run();
				galvo.home();
				laser.setVoltage(0.6);
				laser.openGate();
				break;
			case 'b':
				break;
			}
			break;

		case KEYBOARD:
			cout << "You can now use keyboard for control, press C to exit" << endl;
			keyboard(galvo, stage);
			break;

		case LASER_PREP:
			laser.setVoltage(0.6);
			laser.openGate();
			galvo.openShutter();
			break;

		case TEST:
			cout << "running test code" << endl;
			test();
			break;

		case LASER:
			cout << "Enter new voltage: ";
			double voltage;
			cin >> voltage;
			laser.setVoltage(voltage);
			cout << "Enter laser state (0 for off, 1 for on): ";
			int state;
			cin >> state;
			if (state == 0) laser.closeGate();
			else if (state == 1) laser.openGate();
			else cout << "Invalid state" << endl;
			break;

		case CONFOCAL:
			cout << " Enter a confocal command: " << endl;
			cout << "f : Set focus depth" << endl;
			cout << "m : Move effector" << endl;
			cout << "r : Move to focus" << endl;
			cout << "b: Back" << endl;
			cout << ">> ";
			char confocal_command;
			cin >> confocal_command;
			if (confocal_command == 'f') {
				confocal.moveEffector("c2l");
				confocal.setFocusDepth();
				confocal.moveEffector("l2c");
			}
			else if (confocal_command == 'm') {
				cout << "Enter direction (c2l or l2c): ";
				string direction;
				cin >> direction;
				confocal.moveEffector(direction);
			}
			else if (confocal_command == 'r') {
				confocal.setState(ConfocalWrapper::LIVE_SCAN);
				confocal.findFocus();
			}
			else if (confocal_command == 'b') {
				break;
			}
			else {
				cout << "Invalid command" << endl;
			}
			break;

		default:
			cout << "Invalid command" << endl;
			break;
		}

	}
	return 0;
}



void test() {
	/*
	MetalMesh mesh = MetalMesh(stage, galvo, laser);
	laser.closeGate();
	laser.setVoltage(1.05);
	mesh.setParameter("voltage", 1.05);
	double kerf = 10;
	cin >> kerf;
	mesh.setParameter("kerf", kerf);
	mesh.setParameter("square_length", 100);
	mesh.setParameter("square_width", 30);
	mesh.setParameter("num_units", 8);
	mesh.setParameter("crosses", 1);
	mesh.raster(300);
	galvo.home();*/
}