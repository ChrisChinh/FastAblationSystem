#pragma once
#include <Windows.h>
#include "GalvoControl.h"
#include "LaserControl.h"
#include <iostream>
#include "StageControl.h"
#define MAX_VELOCITY 150
using namespace std;

bool isAnyKeyPressed();


void keyboard(GalvoControl& galvo, StageControl& stage) {
	bool running = true;
	double velocity = 30;
	double galvo_v = 0;
	double galvo_h = 0;
	bool key_pressed = false;

	while (running) {
		if (GetAsyncKeyState(VK_LEFT) & 0x8000 && !key_pressed) { stage.jogMotion(0, velocity); key_pressed = true; }
		else if (GetAsyncKeyState(VK_RIGHT) & 0x8000 && !key_pressed) { cout << "Jogging" << endl; stage.jogMotion(0, -velocity); key_pressed = true; }
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
		else if (GetAsyncKeyState(VK_PRIOR) & 0x8000 && !key_pressed) { stage.jogMotion(2, velocity / 8.0); key_pressed = true; }
		else if (GetAsyncKeyState(VK_NEXT) & 0x8000 && !key_pressed) { stage.jogMotion(2, -velocity / 8.0); key_pressed = true; }
		else if (GetAsyncKeyState('A') & 0x8000) {
			galvo_h -= 2;
			galvo_h = max(-22.5, galvo_h);
			cout << "Galvo H set to " << galvo_h << endl;
			galvo.setAngle(0, galvo_h);
			Sleep(100);
		}
		else if (GetAsyncKeyState('D') & 0x8000) {
			galvo_h += 2;
			galvo_h = min(22.5, galvo_h);
			cout << "Galvo H set to " << galvo_h << endl;
			galvo.setAngle(0, galvo_h);
			Sleep(100);
		}
		else if (GetAsyncKeyState('W') & 0x8000) {
			galvo_v += 2;
			galvo_v = min(22.5, galvo_v);
			cout << "Galvo V set to " << galvo_v << endl;
			galvo.setAngle(1, galvo_v);
			Sleep(100);
		}
		else if (GetAsyncKeyState('S') & 0x8000) {
			galvo_v -= 2;
			galvo_v = max(-22.5, galvo_v);
			cout << "Galvo V set to " << galvo_v << endl;
			galvo.setAngle(1, galvo_v);
			Sleep(100);
		}
		else if (GetAsyncKeyState('O') & 0x8000 && !key_pressed) {
			galvo.openShutter();
			key_pressed = true;
		}
		else if (GetAsyncKeyState('C') & 0x8000) return;
		else if (!isAnyKeyPressed()) {
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