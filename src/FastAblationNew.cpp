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
#define MAX_VELOCITY 150

using namespace std;

void keyboard();
bool isAnyKeyPressed();
void test();

LaserControl laser = LaserControl();
GalvoControl galvo = GalvoControl();
StageControl stage = StageControl();


enum string_code {
	HELP,
	QUIT,
	EXPERIMENT,
	KEYBOARD,
	INVALID,
	TEST
};

string_code hash_string(string const& inString) {
	if (inString == "h") return HELP;
	if (inString == "q") return QUIT;
	if (inString == "e") return EXPERIMENT;
	if (inString == "k") return KEYBOARD;
	if (inString == "t") return TEST;
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
			break;

		case QUIT:
			return 0;

		case EXPERIMENT:
			cout << "not yet implemented" << endl;
			break;

		case KEYBOARD:
			cout << "You can now use keyboard for control, press C to exit" << endl;
			keyboard();
			break;

		case TEST:
			cout << "running test code" << endl;
			test();
			break;

		default:
			cout << "Invalid command" << endl;
			break;
		}
	}
	keyboard();
	return 0;
}

void keyboard() {
	bool running = true;
	double velocity = 30;
	bool key_pressed = false;

	while (running) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 && !key_pressed) {stage.jogMotion(0, velocity); key_pressed = true; }
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && !key_pressed) { cout << "Jogging" << endl; stage.jogMotion(0, -velocity); key_pressed = true;}
		else if (GetAsyncKeyState(VK_DOWN) & 0x8000 && !key_pressed) { stage.jogMotion(1, velocity); key_pressed = true; }
		else if (GetAsyncKeyState(VK_UP) & 0x8000 && !key_pressed) { stage.jogMotion(1, -velocity); key_pressed = true; }
		else if (GetAsyncKeyState(VK_ADD) & 0x8000 && !key_pressed) {
			key_pressed = true;
			velocity *= 1.1;
			if (velocity > MAX_VELOCITY) {
				velocity = MAX_VELOCITY;
			}
			else if (velocity < 0) velocity = 0.01;
			cout << "New velocity: " << velocity << endl;
		}
		else if (GetAsyncKeyState(VK_SUBTRACT) & 0x8000 && !key_pressed) {
			key_pressed = true;
			velocity *= 0.8;
			if (velocity < 0) velocity = 0.01;
			cout << "New velocity: " << velocity << endl;
		}
		else if (GetAsyncKeyState(VK_MULTIPLY) & 0x8000 && !key_pressed) {
			key_pressed = true;
			velocity = 0.8;
			cout << "New velocity: " << velocity << endl;
		}
		else if (GetAsyncKeyState(VK_DIVIDE) & 0x8000 && !key_pressed) { 
			key_pressed = true;
			velocity = 130;
			cout << "New velocity: " << velocity << endl;
		}
		else if (GetAsyncKeyState(VK_PRIOR) & 0x8000 && !key_pressed) { stage.jogMotion(2,velocity / 8.0); key_pressed = true; }
		else if (GetAsyncKeyState(VK_NEXT) & 0x8000 && !key_pressed) { stage.jogMotion(2, -velocity / 8.0); key_pressed = true; }
		else if (GetAsyncKeyState('C') & 0x8000) return;
		else if (!isAnyKeyPressed()){
			key_pressed = false;
			stage.stopJogMotion(0);
			stage.stopJogMotion(1);
			stage.stopJogMotion(2);
		}
	}
}

bool isAnyKeyPressed() {
	for (int key = 0x08; key <= 0xFE; ++key) { // Loop through all possible key codes
		if (GetAsyncKeyState(key) & 0x8000) {
			return true; // If any key is pressed, return true
		}
	}
	return false; // No keys are pressed
}

void test() {
	MetalMesh mesh = MetalMesh(stage, galvo, laser);
	laser.setVoltage(1.05);
	mesh.setParameter("voltage", 1.05);
	double kerf = 10;
	cin >> kerf;
	mesh.setParameter("kerf", kerf);
	mesh.setParameter("square_length", 100);
	mesh.setParameter("square_width", 100);
	mesh.drawSquare(0, 0);
}