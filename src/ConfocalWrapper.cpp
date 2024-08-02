#include "ConfocalWrapper.h"
#include <fstream>
#include <iostream>

ConfocalWrapper::ConfocalWrapper(StageControl& stage) : stage(stage) {
	cout << "Confocal initialized" << endl;
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
	if (direction == "c21") {
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

	cout << "Offsets: " << confocalOffsets[0] << ", " << confocalOffsets[1] << ", " << confocalOffsets[2] << endl;
}

void ConfocalWrapper::moveToFocusLS() {
	this->moveEffector("c2l");
	double depth = confocal.getDepth();
	double delta = focus_depth - depth;
	stage.moveRelative(0, 0, delta / 1000);
	this->moveEffector("l2c");
}

double ConfocalWrapper::getDepth() {
	return confocal.getDepth();
}
