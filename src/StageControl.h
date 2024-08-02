#pragma once
#include "../include/ACSC.h"
class StageControl {

public:
	StageControl();
	~StageControl();
	void reconnect();
	void moveAbsolute(double x, double y, double z, int flags = 0, bool async_mode = false);
	void moveRelative(double x, double y, double z, bool async_mode = false);
	double* getStagePosition();
	void jogMotion(int axis, double velocity);
	void stopJogMotion(int axis);
	void setVelocity(double velocity);
	double getVelocity();
	bool isMoving();

private:
	HANDLE handle;
};