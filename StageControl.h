#pragma once
#include "ACSC.h"
class StageControl {

public:
	StageControl();
	~StageControl();
	void reconnect();
	void moveAbsolute(double x, double y, double z, int flags = 0);
	void moveRelative(double x, double y, double z);
	void moveAxisAbsolute(int axis, double position);
	void moveAxisRelative(int axis, double position);
	double* getStagePosition();
	void jogMotion(int axis, double velocity);
	void stopJogMotion(int axis);
	void setVelocity(double velocity);

private:
	HANDLE handle;
};