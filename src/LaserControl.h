#pragma once
#include "DAQControl.h"
#define MAX_VOLTAGE 2.5
class LaserControl
{
	public:
	LaserControl();
	void setVoltage(double v);
	double getVoltage();
	void openGate();
	void closeGate();

	private:
	double voltage;
	DAQControl laser = DAQControl("20BF9C2", 1);
	bool gateOpen = true;


};

