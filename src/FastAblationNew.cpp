// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include "DAQControl.h"
#include "DataReciever.h"
#include <cmath>

using namespace std;

void test();

// Controllers
//DAQControl daq = DAQControl("");

// Experiments



enum string_code {
	HELP,
	QUIT,
	EXPERIMENT,
	KEYBOARD,
	INVALID,
	TEST,
	LASER,
	CONFOCAL,
	LASER_PREP
};


void CreateOutputData(int numberOfSamplesPerChannel, int chanCount,double* buffer, double frequency)
{
	const double twoPI = 2 * 3.14159265358979323846;
	int i, j;

	double min = 0.0;
	double max = 5.0;

	double amplitude =  max - min;
	double f = twoPI * frequency / numberOfSamplesPerChannel;
	double phase = 0.0;
	double offset = (min == 0) ? amplitude/2 : 0;

	for (i=0; i<numberOfSamplesPerChannel; i++)
	{
		for (j=0; j<chanCount; j++)
		{
			*buffer++ = (double) ((sin(phase) * amplitude/2) + offset);
		}

		phase += f;
		if( phase > twoPI )
			phase = phase - twoPI;
	}
}

void GenerateSquareWave(int numSamples, double* buffer) {
	for (int i = 0; i < numSamples; i++) {
		if (i % 2 == 0) {
			buffer[i] = 5.0;
		}
		else {
			buffer[i] = 0.0;
		}
	}
}

void test() {
	DAQControl daq = DAQControl("20BF9C2");
	auto start = daq.getTimeinMicroseconds();
	uint16_t num_iterations = 60000;
	double rate = daq.getIdealRate(num_iterations);
	cout << "Ideal rate according to DAQ: " << rate << " with num iterations: " << num_iterations << endl;
	double buffer[(uint16_t)rate];
	uint16_t totalTime = 0;

	CreateOutputData((uint16_t)rate, 1, buffer, 375);
	for (uint8_t i = 0; i < 100; i++) {
		auto scanStart = daq.getTimeinMicroseconds();
		daq.analogScanOut(0, vector<double>(buffer, buffer + (uint16_t)rate), true, rate);
		auto scanEnd = daq.getTimeinMicroseconds();
		totalTime += (scanEnd - scanStart);
	}

	cout << "Total time taken: " << totalTime / (100 * 1000) << endl;
}


int main()
{
	DataReciever r("10.10.10.10", 50007);
	while (true) {
		auto data = r.receiveData();
		cout << "Received data with " << data.size() << " rows and " << data[0].size() << " columns." << endl;
	}

}