#pragma once
#include "../include/CHRocodileLib.h"

class ConfocalControl
{
private:
	Conn_h hCHR;
	void connect();
public:
	ConfocalControl();
	~ConfocalControl();
	double getDepth();
	void reconnect();
};