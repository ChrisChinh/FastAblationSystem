#pragma once
#include "DAQControl.h"
class GalvoControl
{
	public:
	GalvoControl();
	void openShutter();
	void closeShutter();
	void setMicron(double h, double v);
	void setAngle(int channel, double x);
	void home();

	private:
	DAQControl galvo = DAQControl("2070C2A", 0);
	void setVoltage(int channel, double v);
};

