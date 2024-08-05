#pragma once
#include "src/GalvoControl.h"
#include "src/LaserControl.h"
#include "src/StageControl.h"
#include "src/ConfocalWrapper.h"
#include <map>
class MetalMesh
{
private:
	StageControl& stage;
	GalvoControl& galvo;
	LaserControl& laser;
	ConfocalWrapper& confocal;
	std::map<std::string, double> parameters;
public:
	void raster(double FOV_length);
	void drawSquare(double h, double v, double height = 0, double width = 0);
	void drawCross(double h, double v);
//public:
	MetalMesh(StageControl& s, GalvoControl& g, LaserControl& l, ConfocalWrapper& c);
	void setParameter(std::string name, double value);
	void loadParameters(std::string filename);
	double getParameter(std::string name);
	int run();



};

