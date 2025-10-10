#include "ConfocalWrapper.h"
#include <fstream>
#include <iostream>
#include <algorithm>

ConfocalWrapper::ConfocalWrapper(StageControl& stage) : stage(stage) {
	try {
		ifstream file("previous_focus_depth.txt");
		if (file.is_open()) {
			string depth;
			getline(file, depth);
			focus_depth = stod(depth);
			file.close();
		}
		else {
			cout << "Unable to open file" << endl;
		}
	}
	catch (exception e) {
		cout << "Error reading file" << endl;
	}
}

void ConfocalWrapper::setFocusDepth() {
	focus_depth = confocal.getDepth();
	if (focus_depth == 0) {
		cout << "Error getting focus depth" << endl;
		return;
	}
	ofstream file("previous_focus_depth.txt");
	if (file.is_open()) {
		file << focus_depth;
		file.close();
	}
	else {
		cout << "Unable to open file" << endl;
	}
}

void ConfocalWrapper::moveEffector(string direction) {
	this->getConfocalOffsets();
	double prev_v = stage.getVelocity();
	stage.setVelocity(200);
	if (direction == "c2l") {
		stage.moveRelative(confocalOffsets[0], confocalOffsets[1], confocalOffsets[2]);
	}
	else if (direction == "l2c") {
		stage.moveRelative(-confocalOffsets[0], -confocalOffsets[1], -confocalOffsets[2]);
	}
	else {
		cout << "Invalid direction" << endl;
	}
	stage.setVelocity(prev_v);
}

void ConfocalWrapper::getConfocalOffsets() {
	string offsets = "";
	try {
		ifstream file("confocal_offsets.txt");
		if (file.is_open()) {
			getline(file, offsets);
			file.close();
		}
		else {
			cout << "Unable to open file" << endl;
		}
	}
	catch (exception e) {
		cout << "Error reading file" << endl;
	}
	if (offsets == "") {
		cout << "No offsets found" << endl;
		return;
	}

	// Extract information from string
	string delimiter = ",";
	string x = offsets.substr(0, offsets.find(delimiter));
	offsets.erase(0, offsets.find(delimiter) + delimiter.length());
	string y = offsets.substr(0, offsets.find(delimiter));
	offsets.erase(0, offsets.find(delimiter) + delimiter.length());
	string z = offsets;
	confocalOffsets[0] = stod(x);
	confocalOffsets[1] = stod(y);
	confocalOffsets[2] = stod(z);
}

void ConfocalWrapper::moveToFocusLS() {
	this->moveEffector("c2l");
	this->moveToFocus();
	this->moveEffector("l2c");
}

void ConfocalWrapper::moveToFocus() {
	double depth = confocal.getDepth();
	double delta = focus_depth - depth;
	cout << "Diff: " << delta / 1000.0 << endl;
	stage.moveRelative(0, 0, delta / 1000);
}

double ConfocalWrapper::getDepth() {
	return confocal.getDepth();
}

void ConfocalWrapper::scanArea(double extent_x, double extent_y) {

	// First, get velocity and set new velocity
	this->moveEffector("c2l");
	double prev_v = stage.getVelocity();
	stage.setVelocity(200);

	// Get current position
	double* pos = stage.getStagePosition();

	cout << "beginning scan of area " << extent_x << " x " << extent_y << endl;

	// Scan area in a grid-like fasion, with one x line per half millimeter
	int direction = 1;
	for (double y = pos[1]; y < pos[1] + extent_y; y += 0.5) {
		stage.moveAbsolute(pos[0], y, pos[2]);
		this->moveToFocus();
		double z = stage.getStagePosition()[2];
		stage.moveRelative(extent_x, 0, 0, true);
		while (stage.isMoving()) {
			double* point = stage.getStagePosition();
			double depth = confocal.getDepth();
			double avgX = (stage.getStagePosition()[0] + point[0]) / 2.0;
			double delta = focus_depth - depth;
			if (avgX - mesh_points.back()[0] < 0.25) {
				continue;
			}
			mesh_points.push_back( { avgX - confocalOffsets[0], y - confocalOffsets[1], z + delta - confocalOffsets[2]} );
		}
	}

	cout << "Scan complete with " << mesh_points.size() << " points" << endl;
	stage.setVelocity(prev_v);
}

double ConfocalWrapper::getDepthAtPoint(double x, double y) {
	if (mesh_points.size() == 0) {
		cout << "No mesh points" << endl;
		return 0;
	}

	// Find the closest point in the mesh
	const double max_dist = 1.0;
	sort(mesh_points.begin(), mesh_points.end(), [x, y](vector<double> a, vector<double> b) {
		double dist_a = sqrt(pow(a[0] - x, 2) + pow(a[1] - y, 2));
		double dist_b = sqrt(pow(b[0] - x, 2) + pow(b[1] - y, 2));
		return dist_a < dist_b;
	});
	return mesh_points[0][2];

}

void ConfocalWrapper::moveToFocusPS() {
	double* pos = stage.getStagePosition();
	double newZ = this->getDepthAtPoint(pos[0], pos[1]);
	stage.moveAbsolute(pos[0], pos[1], newZ);
}

void ConfocalWrapper::findFocus() {
	switch (confocal_state) {
	case NO_CONFOCAL:
		return;
		break;
	case PRE_SCAN:
		this->moveToFocusPS();
		break;
	case LIVE_SCAN:
		this->moveToFocusLS();
		break;
	}
}

void ConfocalWrapper::setState(confocal_code state) {
	confocal_state = state;
}