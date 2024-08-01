// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include "StageControl.h"
#include "LaserControl.h"
#include "GalvoControl.h"
#include <wchar.h>
#define MAX_VELOCITY 150

using namespace std;
void mainloop();
bool isAnyKeyPressed();


int main() {
	mainloop();
	return 0;
}

void mainloop() {
	bool running = true;
	double velocity = 30;
	StageControl stage = StageControl();
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