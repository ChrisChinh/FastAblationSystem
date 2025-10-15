// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "GalvoControl.h"
#include <stdint.h>
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


void CreateOutputData(int numberOfSamplesPerChannel, int chanCount,double* buffer)
{
	const double twoPI = 2 * 3.14159265358979323846;
	int i, j;

	double min = 0.0;
	double max = 5.0;

	double amplitude =  max - min;
	double f = twoPI / numberOfSamplesPerChannel;
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

int main() {
	DAQControl daq = DAQControl("20BF9C2");
	double buffer[1000];
	GenerateSquareWave(1000, buffer);
	uint64_t totalTime = 0;
	int j = 0;
	while (j < 10000) {
	for (int i = 0; i < 1000; i++) {
		uint64_t start = daq.getTimeinMicroseconds();
		daq.setAnalogOut(0, buffer[i]);
		uint64_t end = daq.getTimeinMicroseconds();
		totalTime += end - start;
	}
	j ++;
}
	cout << "Total time taken: " << totalTime << endl;

	return 0;
}