#include "LaserControl.h"
#include <iostream>
using namespace std;
LaserControl::LaserControl() {
	voltage = 0;
	laser.setAnalogOut(1, 0);
}

void LaserControl::closeGate() {
	gateOpen = false;
	laser.setDigitalOut(7, 0);
}

void LaserControl::openGate() {
	gateOpen = true;
	laser.setDigitalOut(7, 1);
}

double LaserControl::getVoltage() {
	return voltage;
}

void LaserControl::setVoltage(double new_voltage) {
	if (new_voltage > MAX_VOLTAGE) new_voltage = MAX_VOLTAGE;
	else if (new_voltage < 0) new_voltage = 0;

	voltage = new_voltage;
	laser.setAnalogOut(1, voltage);
}
