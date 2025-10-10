// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GalvoControl.h"

using namespace std;

void test();

// Controllers
GalvoControl galvo = GalvoControl();

// Experiments



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

int main() {
	return 0;
}