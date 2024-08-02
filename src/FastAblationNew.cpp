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

LaserControl laser = LaserControl();
GalvoControl galvo = GalvoControl();
StageControl stage = StageControl();
ConfocalWrapper confocal = ConfocalWrapper(stage);


enum string_code {
	HELP,
	QUIT,
	EXPERIMENT,
	KEYBOARD,
	INVALID,
	TEST,
	LASER,
	CONFOCAL
};

string_code hash_string(string const& inString) {
	if (inString == "h") return HELP;
	if (inString == "q") return QUIT;
	if (inString == "e") return EXPERIMENT;
	if (inString == "k") return KEYBOARD;
	if (inString == "t") return TEST;
	if (inString == "l") return LASER;
	if (inString == "c") return CONFOCAL;
	return INVALID;
}

int main() {
	galvo.openShutter();
	laser.setVoltage(0.7);
	laser.openGate();
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
			break;

		case QUIT:
			return 0;

		case EXPERIMENT:
			cout << "not yet implemented" << endl;
			break;

		case KEYBOARD:
			cout << "You can now use keyboard for control, press C to exit" << endl;
			keyboard(galvo, stage);
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
			cout << "Getting confocal measurement" << endl;
			double z;
			z = confocal.getDepth();
			cout << "Depth: " << z << endl;
			break;

		default:
			cout << "Invalid command" << endl;
			break;
		}
	}
	return 0;
}



void test() {
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
	galvo.home();
}