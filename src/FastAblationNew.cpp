// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include "DAQControl.h"
#include "DataReciever.h"
#include <cmath>
#include <thread>
#include <chrono>
using namespace std;

#define LASER_PIN 7
#define X_PIN 0
#define Y_PIN 1
#define IDEAL_RATE 9300 // Experimentally determined
#define SOLENOID_PIN 4
#define X_BIAS 2.5
#define Y_BIAS 2.5
#define WAIT_SIGNAL 200
#define WAIT_PERIOD 200 // us


typedef enum {
	COMMAND_ABLATE_BUFFER,
	COMMAND_GET_RATE,
	COMMAND_LASER_ON,
	COMMAND_LASER_OFF,
	COMMAND_GALVO_HOME,
	COMMAND_SET_GALVO_X,
	COMMAND_SET_GALVO_Y,
	COMMAND_GET_GALVO_POS,
	COMMAND_ENABLE_SOLENOID,
	COMMAND_DISABLE_SOLENOID
} CommandType;

// Globals
DAQControl daq = DAQControl();
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

		if (rows != 4) {
			cout << "Expected 4 rows of data (for 4 channels), but received " << rows << " rows." << endl;
			return;
		}

		if (cols < 2) {
			// Skip ablation if there are not enough points
			r.sendDouble(1.0);
			return;
		}
		
		int bufferSize = cols;
		int speed = data[0][0]; // First element is speed
		for (int i = 1; i < bufferSize; i++) {
			// Unpack the points for each line
			double x1 = data[0][i];
			double y1 = data[1][i];
			double x2 = data[2][i];
			double y2 = data[3][i];
			if (isinf(x1)){
				if (signbit(x1)) {
					daq.setDigitalOut(LASER_PIN, false); // Laser off
				}
				else {
					daq.setDigitalOut(LASER_PIN, true); // Laser on
				}
				continue;
			}
			else if (x1 == WAIT_SIGNAL) {
				// Wait for a period
				std::this_thread::sleep_for(std::chrono::microseconds(WAIT_PERIOD));
			}
			if (X_PIN == 0)
				daq.drawLine(x1, y1, x2, y2, speed, rate);
			else
				daq.drawLine(y1, x1, y2, x2, speed, rate);

		}
		r.sendDouble(1.0); // Acknowledge receipt
}


inline void repl(double rate) {
	CommandType command = (CommandType)r.receiveInt();
	switch (command) {
		case COMMAND_ABLATE_BUFFER:
		{
			r.sendDouble(1.0); // Acknowledge command receipt
			ablateBuffer(rate);
			break;
		}
		case COMMAND_GET_RATE:
		{
			r.sendDouble(rate);
			break;
		}
		case COMMAND_LASER_ON:
		{
			daq.setDigitalOut(LASER_PIN, true);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_LASER_OFF:
		{
			daq.setDigitalOut(LASER_PIN, false);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_GALVO_HOME:
		{
			daq.setAnalogOut(X_PIN, 0.0);
			daq.setAnalogOut(Y_PIN, 0.0);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_SET_GALVO_X:
		{
			double x = r.receiveDouble();
			daq.setAnalogOut(X_PIN, x);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_SET_GALVO_Y:
		{
			double y = r.receiveDouble();
			daq.setAnalogOut(Y_PIN, y);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_GET_GALVO_POS:
		{
			double x = daq.getVoltage(X_PIN);
			double y = daq.getVoltage(Y_PIN);
			r.sendDouble(x);
			r.sendDouble(y);
			break;
		}
		case COMMAND_ENABLE_SOLENOID:
		{
			// Assuming solenoid is controlled via a digital output pin, e.g., pin 8
			daq.setDigitalOut(SOLENOID_PIN, true);
			r.sendDouble(1.0);
			break;
		}
		case COMMAND_DISABLE_SOLENOID:
		{
			daq.setDigitalOut(SOLENOID_PIN, false);
			r.sendDouble(1.0);
			break;
		}
	}

}

int main()
{
	double rate = IDEAL_RATE; //daq.getIdealRate_all(60000);
	daq.setBias(X_PIN, X_BIAS);
	daq.setBias(Y_PIN, Y_BIAS);


	while (true) {
		repl(rate);
	}
	return 0;

}