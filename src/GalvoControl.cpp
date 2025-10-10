#include "GalvoControl.h"
#define MICRON_TO_ANGLE_X 0.02695
#define MICRON_TO_ANGLE_Y 0.02774
#define ANGLE_TO_VOLTAGE 0.222
GalvoControl::GalvoControl() {
	galvo.setDigitalOut(7, 1);
	GalvoControl::home();
}

void GalvoControl::closeShutter() {
	galvo.setDigitalOut(6, 0);
}

void GalvoControl::openShutter() {
	int result = galvo.setDigitalOut(6, 1);
}

void GalvoControl::home() {
	galvo.setAnalogOut(0, 0);
	galvo.setAnalogOut(1, 0);
}

void GalvoControl::setMicron(double h, double v) {
	GalvoControl::setAngle(0, h * MICRON_TO_ANGLE_X);
	GalvoControl::setAngle(1, v * MICRON_TO_ANGLE_Y);
}

void GalvoControl::setAngle(int channel, double value) {
	this->setVoltage(channel, value * ANGLE_TO_VOLTAGE);
}

void GalvoControl::setVoltage(int channel, double v) {
	if (v > 5) v = 5;
	else if (v < -5) v = -5;
	galvo.setAnalogOut(channel, v);
}