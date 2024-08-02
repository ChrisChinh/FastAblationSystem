#pragma once
#include "ConfocalControl.h"
#include "StageControl.h"
#include <vector>
#include <string>
#include <iostream>
using namespace std;

class ConfocalWrapper
{
private:
	ConfocalControl confocal = ConfocalControl();
	StageControl& stage;
	double focus_depth = 0;
	double confocalOffsets[3] = { 0, 0, 0 };
	vector<vector<double>> mesh_points;
	void getConfocalOffsets();
public:
	ConfocalWrapper(StageControl& stage);
	void setFocusDepth();
	void moveEffector(string direction);
	void moveToFocusLS();
	double getDepth();
};

