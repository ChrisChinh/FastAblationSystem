#include "MetalMesh.h"
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
using namespace std;


MetalMesh::MetalMesh(StageControl& s, GalvoControl& g, LaserControl& l, ConfocalWrapper& c) : stage(s), galvo(g), laser(l), confocal(c)
{
}

int MetalMesh::run() {
	laser.setVoltage(parameters["voltage"]);
	double* origin = stage.getStagePosition();

	double FOV_length = parameters["period}"] * parameters["num_units"]; // in microns
	int rows = parameters["aperture"] / (FOV_length / 1000.0);
	int cols = rows; // For now, everything is square

	// Decide on confocal usage
	if (parameters["use_confocal"] == 1) {
		if (parameters["pre_scan"] == 1) confocal.setState(ConfocalWrapper::PRE_SCAN);
		else confocal.setState(ConfocalWrapper::LIVE_SCAN);
	}
	else confocal.setState(ConfocalWrapper::NO_CONFOCAL);

	cout << "Metal mesh ablation started" << endl;
	int total_fields = rows * cols;
	auto start = chrono::high_resolution_clock::now();

	for (int x = 0; x < cols; x++) {
		for (int y = 0; y < rows; y++) {
			float x_pos = origin[0] - (x * FOV_length);
			float y_pos = origin[1] - (y * FOV_length);

			// Check if in the bounds of the circle
			if (parameters["circle_aperture"] == 1) {
				double h = FOV_length * (x - cols / 2);
				double k = FOV_length * (y - rows / 2);
				double current_radius = sqrt(h * h + k * k);
				if (current_radius > parameters["aperture"] / 2) {
					continue;
				}
			}

			confocal.findFocus();
			stage.moveAbsolute(x_pos, y_pos, origin[2]);
			cout << "FOV: " << x << ", " << y << endl;
			this->raster(FOV_length);
		}

	}
	return 1;
}

void MetalMesh::raster(double FOV_length) {
	/*
	* Raster the current field, using the galvo to scan the laser across the field
	* and draw a pattern of either crosses or squares according to the parameters set
	* */
	double square_length = parameters["square_length"] - parameters["kerf"];
	double square_width = parameters["square_width"] - parameters["kerf"];
	laser.closeGate();

	// Go to each spot where a square should be drawn
	for (double v = (-FOV_length / 2); v < (FOV_length / 2); v += (FOV_length / parameters["num_units"])) {
		for (double h = (-FOV_length / 2); h < (FOV_length / 2); h += (FOV_length / parameters["num_units"])) {
			cout << "Drawing square at " << h << ", " << v << endl;
			galvo.setMicron(h, v);
			if (parameters["crosses"] == 1)
				drawCross(h, v);
			else drawSquare(h, v);
		}
	}

}

void MetalMesh::drawSquare(double h_init, double v_init, double width, double height) {
	double square_length = (height == 0) ? parameters["square_length"] - parameters["kerf"] : height;
	double square_width = (width == 0) ? parameters["square_width"] - parameters["kerf"] : width;

	laser.openGate();
	for (double v = (-square_length / 2) + v_init; v < (square_length / 2) + v_init; v += parameters["kerf"]) {
		for (double h = (-square_width / 2) + h_init; h < (square_width / 2) + h_init; h += parameters["kerf"]) {
			galvo.setMicron(h, v);
		}
	}
	laser.closeGate();
}

void MetalMesh::drawCross(double h, double v) {
	cout << "Drawing cross" << endl;
	double length = parameters["square_length"] - parameters["kerf"];
	double width = parameters["square_width"] - parameters["kerf"];

	this->drawSquare(h, v);
	this->drawSquare(h, v, length, width);
}

void MetalMesh::setParameter(string name, double value) {
	parameters[name] = value;
}

void MetalMesh::loadParameters(string filename) {
	// Load parameters from a file
	ifstream file(filename);
	string line;
	while (getline(file, line)) {
		line.erase(remove_if(line.begin(), line.end(), isspace), line.end());
		string name = line.substr(0, line.find('='));
		string value = line.substr(line.find('=') + 1);
		cout << "Setting parameter " << name << " to " << value << endl;
		parameters[name] = stod(value);
	}
}