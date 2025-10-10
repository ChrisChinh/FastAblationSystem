#pragma once
#include "ConfocalControl.h"
#include "StageControl.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class ConfocalWrapper
{
public:
	ConfocalWrapper(StageControl& stage);
	void setFocusDepth();
	void moveEffector(string direction);
	double getDepth();
	void scanArea(double extent_x, double extent_y);
	void findFocus();
	enum confocal_code {
		NO_CONFOCAL,
		PRE_SCAN,
		LIVE_SCAN
	};
	void setState(confocal_code state);
private:
	ConfocalControl confocal = ConfocalControl();
	StageControl& stage;
	double focus_depth = 0;
	double confocalOffsets[3] = { 0, 0, 0 };
	vector<vector<double>> mesh_points;
	void getConfocalOffsets();
	void moveToFocus();
	double getDepthAtPoint(double x, double y);
	confocal_code confocal_state = NO_CONFOCAL;
	void moveToFocusPS();
	void moveToFocusLS();


};

