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


void ConvertRangeToMinMax(Range range, double* min, double* max)
{
	switch(range)
	{
	case(BIP60VOLTS):
		*min = -60.0;
		*max = 60.0;
		break;
	case(BIP30VOLTS):
		*min = -30.0;
		*max = 30.0;
		break;
	case(BIP15VOLTS):
		*min = -15.0;
		*max = 15.0;
		break;
	case(BIP20VOLTS):
		*min = -20.0;
		*max = 20.0;
		break;
	case(BIP10VOLTS):
		*min = -10.0;
		*max = 10.0;
		break;
	case(BIP5VOLTS):
		*min = -5.0;
		*max = 5.0;
		break;
	case(BIP4VOLTS):
		*min = -4.0;
		*max = 4.0;
		break;
	case(BIP2PT5VOLTS):
		*min = -2.5;
		*max = 2.5;
		break;
	case(BIP2VOLTS):
		*min = -2.0;
		*max = 2.0;
		break;
	case(BIP1PT25VOLTS):
		*min = -1.25;
		*max = 1.25;
		break;
	case(BIP1VOLTS):
		*min = -1.0;
		*max = 1.0;
		break;
	case(BIPPT625VOLTS):
		*min = -0.625;
		*max = 0.625;
		break;
	case(BIPPT5VOLTS):
		*min = -0.5;
		*max = 0.5;
		break;
	case(BIPPT25VOLTS):
		*min = -0.25;
		*max = 0.25;
		break;
	case(BIPPT125VOLTS):
		*min = -0.125;
		*max = 0.125;
		break;
	case(BIPPT2VOLTS):
		*min = -0.2;
		*max = 0.2;
		break;
	case(BIPPT1VOLTS):
		*min = -0.1;
		*max = 0.1;
		break;
	case(BIPPT078VOLTS):
		*min = -0.078;
		*max = 0.078;
		break;
	case(BIPPT05VOLTS):
		*min = -0.05;
		*max = 0.05;
		break;
	case(BIPPT01VOLTS):
		*min = -0.01;
		*max = 0.01;
		break;
	case(BIPPT005VOLTS):
		*min = -0.005;
		*max = 0.005;
		break;
	case(BIP3VOLTS):
		*min = -3.0;
		*max = 3.0;
		break;
	case(BIPPT312VOLTS):
		*min = -0.312;
		*max = 0.312;
		break;
	case(BIPPT156VOLTS):
		*min = -0.156;
		*max = 0.156;
		break;

	case(UNI60VOLTS):
		*min = 0.0;
		*max = 60.0;
		break;
	case(UNI30VOLTS):
		*min = 0.0;
		*max = 30.0;
		break;
	case(UNI15VOLTS):
		*min = 0.0;
		*max = 15.0;
		break;
	case(UNI20VOLTS):
		*min = 0.0;
		*max = 20.0;
		break;
	case(UNI10VOLTS):
		*min = 0.0;
		*max = 10.0;
		break;
	case(UNI5VOLTS):
		*min = 0.0;
		*max = 5.0;
		break;
	case(UNI4VOLTS):
		*min = 0.0;
		*max = 4.0;
		break;
	case(UNI2PT5VOLTS):
		*min = 0.0;
		*max = 2.5;
		break;
	case(UNI2VOLTS):
		*min = 0.0;
		*max = 2.0;
		break;
	case(UNI1PT25VOLTS):
		*min = 0.0;
		*max = 1.25;
		break;
	case(UNI1VOLTS):
		*min = 0.0;
		*max = 1.0;
		break;
	case(UNIPT625VOLTS):
		*min = 0.0;
		*max = 0.625;
		break;
	case(UNIPT5VOLTS):
		*min = 0.0;
		*max = 0.5;
		break;
	case(UNIPT25VOLTS):
		*min = 0.0;
		*max = 0.25;
		break;
	case(UNIPT125VOLTS):
		*min = 0.0;
		*max = 0.125;
		break;
	case(UNIPT2VOLTS):
		*min = 0.0;
		*max = 0.2;
		break;
	case(UNIPT1VOLTS):
		*min = 0.0;
		*max = 0.1;
		break;
	case(UNIPT078VOLTS):
		*min = 0.0;
		*max = 0.078;
		break;
	case(UNIPT05VOLTS):
		*min = 0.0;
		*max = 0.05;
		break;
	case(UNIPT01VOLTS):
		*min = 0.0;
		*max = 0.1;
		break;
	case(UNIPT005VOLTS):
		*min = 0.0;
		*max = 0.005;
		break;
	case(MA0TO20):
		*min = 0.0;
		*max = 20.0;
		break;
	}
}

void CreateOutputData(int numberOfSamplesPerChannel, int chanCount, Range range, double* buffer)
{
	const double twoPI = 2 * 3.14159265358979323846;
	int i, j;

	double min = 0.0;
	double max = 0.0;
	ConvertRangeToMinMax(range, &min, &max);

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

int main() {
	DAQControl daq = DAQControl("20BF9C2");
	double buffer[1000];
	CreateOutputData(500, 2, BIP10VOLTS, buffer);
	vector<double> data(buffer, buffer+1000);
	daq.analogScanOut(0, 1, data, true);
	return 0;
}