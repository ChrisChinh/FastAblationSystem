// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include "DAQControl.h"
#include "DataReciever.h"
#include <cmath>
using namespace std;


typedef enum {
	COMMAND_ABLATE_BUFFER,
	COMMAND_GET_RATE,
	COMMAND_LASER_ON,
	COMMAND_LASER_OFF,
	COMMAND_GALVO_HOME,
	COMMAND_SET_GALVO_X,
	COMMAND_SET_GALVO_Y,
	COMMAND_GET_GALVO_POS
} CommandType;


// Globals
DAQControl daq = DAQControl("20BF9C2");
DataReciever r("10.10.10.10", 50007);

void ablateBuffer(double rate) {
		auto data = r.receiveData();
		if (data.size() == 0) {
			cout << "No data received, reconnecting..." << endl;
			r.reconnect();
			return;
		}
		int rows = data.size();
		int cols = data[0].size();
		cout << "Received data with " << data.size() << " rows and " << data[0].size() << " columns." << endl;

		if (rows != 4) {
			cout << "Expected 4 rows of data (for 4 channels), but received " << rows << " rows." << endl;
			return;
		}

		if (cols < 2) {
			cout << "Expected at least 1 point and header information, but received " << cols << " columns." << endl;
			cout << "Skipping ablation" << endl;
			r.sendDouble(1.0);
			return;
		}
		
		int bufferSize = cols;
		int speed = data[0][0]; // First element is speed
		cout << "Drawing " << (bufferSize - 1) << " lines at speed " << speed << " um/s." << endl;
		for (int i = 1; i < bufferSize; i++) {
			// Unpack the points for each line
			double x1 = data[0][i];
			double y1 = data[1][i];
			double x2 = data[2][i];
			double y2 = data[3][i];
			cout << "Line " << (i - 1) << ": (" << x1 << ", " << y1 << ") to (" << x2 << ", " << y2 << ")" << endl;
			if (x1 == INFINITY && y1 == INFINITY && x2 == INFINITY && y2 == INFINITY) {
				//This means turn on the laser
				daq.setDigitalOut(7, true);
				continue;
			}
			else if (x1 == -INFINITY && y1 == -INFINITY && x2 == -INFINITY && y2 == -INFINITY) {
				//This means turn off the laser
				daq.setDigitalOut(7, false);
				continue;
			}
			
			daq.drawLine(x1, y1, x2, y2, speed, rate);

		}
		r.sendDouble(1.0); // Acknowledge receipt
}


inline void repl(double rate) {
	CommandType command = (CommandType)r.receiveInt();
	switch (command) {
		case COMMAND_ABLATE_BUFFER:
		{
			cout << "Ablating buffer..." << endl;
			r.sendDouble(1.0); // Acknowledge command receipt
			ablateBuffer(rate);
			break;
		}
		case COMMAND_GET_RATE:
		{
			cout << "Sending ideal rate to client." << endl;
			r.sendDouble(rate);
			break;
		}
		case COMMAND_LASER_ON:
		{
			cout << "Turning laser on." << endl;
			daq.setDigitalOut(7, true);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_LASER_OFF:
		{
			cout << "Turning laser off." << endl;
			daq.setDigitalOut(7, false);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_GALVO_HOME:
		{
			cout << "Homing galvos." << endl;
			daq.setAnalogOut(0, 0.0);
			daq.setAnalogOut(1, 0.0);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_SET_GALVO_X:
		{
			cout << "Setting galvo X position." << endl;
			double x = r.receiveDouble();
			daq.setAnalogOut(0, x);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_SET_GALVO_Y:
		{
			cout << "Setting galvo Y position." << endl;
			double y = r.receiveDouble();
			daq.setAnalogOut(1, y);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_GET_GALVO_POS:
		{
			cout << "Getting galvo positions." << endl;
			double x = daq.getVoltage(0);
			double y = daq.getVoltage(1);
			r.sendDouble(x);
			r.sendDouble(y);
			break;
		}
	}

}

int main()
{
	double rate = 9300; //daq.getIdealRate_all(60000);
	cout << "Ideal rate according to DAQ: " << rate << endl;
	while (true) {
		repl(rate);
	}
	return 0;

}