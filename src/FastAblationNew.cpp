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

void GenerateSquareWave(int numSamples, bool* buffer, double frequency) {
	int samplePeriod = (int)(numSamples / frequency);
	for (int i = 0; i < numSamples; i++) {
		if ((i % samplePeriod) < (samplePeriod / 2)) {
			buffer[i] = true;
		}
		else {
			buffer[i] = false;
		}
	}
}

void GenerateTriangleWave(int numberOfSamplesPerSecond, double frequency, double* buffer)
{
	double min = 0.0;
	double max = 5.0;

	double amplitude =  max - min;
	double numberOfSamplesPerPeriod = numberOfSamplesPerSecond / frequency;

	for (int i=0; i<numberOfSamplesPerSecond; i++)
	{
		int phase_index = i % (int)numberOfSamplesPerPeriod;
		if (phase_index < numberOfSamplesPerPeriod / 2)
		{
			buffer[i] = (double)(min + (phase_index * amplitude / (numberOfSamplesPerPeriod / 2)));
		}
		else
		{
			buffer[i] = (double)(max - ((phase_index - numberOfSamplesPerPeriod / 2) * amplitude / (numberOfSamplesPerPeriod / 2)));
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

void triangleWaveTest(){
	DAQControl daq = DAQControl("20BF9C2");
	cout << "Starting triangle wave test..." << endl;
	auto start = daq.getTimeinMicroseconds();
	uint16_t num_iterations = 60000;
	double rate = daq.getIdealRate(num_iterations);
	cout << "Ideal rate according to DAQ: " << rate << " with num iterations: " << num_iterations << endl;
	double buffer[(uint16_t)rate];
	uint16_t totalTime = 0;

	GenerateTriangleWave((uint16_t)rate, 375, buffer);
	for (uint8_t i = 0; i < 100; i++) {
		auto scanStart = daq.getTimeinMicroseconds();
		daq.analogScanOut(0, vector<double>(buffer, buffer + (uint16_t)rate), true, rate);
		auto scanEnd = daq.getTimeinMicroseconds();
		totalTime += (scanEnd - scanStart);
	}

	cout << "Total time taken: " << totalTime / (100 * 1000) << endl;
}

void two_triangles_test(){
	DAQControl daq = DAQControl("20BF9C2");
	cout << "Starting triangle wave test..." << endl;
	auto start = daq.getTimeinMicroseconds();
	uint16_t num_iterations = 60000;
	double rate = daq.getIdealRate_all(num_iterations);
	cout << "Ideal rate according to DAQ: " << rate << " with num iterations: " << num_iterations << endl;
	double triangle_buffer_375[(uint16_t)rate];
	double triangle_buffer_550[(uint16_t)rate];
	uint16_t totalTime = 0;

	GenerateTriangleWave((uint16_t)rate, 375, triangle_buffer_375);
	GenerateTriangleWave((uint16_t)rate, 550, triangle_buffer_550);
	for (uint8_t i = 0; i < 100; i++) {
		auto scanStart = daq.getTimeinMicroseconds();
		daq.analogScanOut_all_given_two_buffers(triangle_buffer_375, triangle_buffer_550, (uint16_t)rate, true, rate);
		auto scanEnd = daq.getTimeinMicroseconds();
		totalTime += (scanEnd - scanStart);
	}

	cout << "Total time taken: " << totalTime / (100 * 1000) << endl;
}

void zipped_triangles_test(){
	// setup
	DAQControl daq = DAQControl("20BF9C2");
	cout << "Starting triangle wave test..." << endl;
	auto start = daq.getTimeinMicroseconds();
	uint16_t num_iterations = 60000;
	double rate = daq.getIdealRate_all(num_iterations);
	cout << "Ideal rate according to DAQ: " << rate << " with num iterations: " << num_iterations << endl;
	
	// create buffers
	double triangle_buffer_375[(uint16_t)rate];
	double triangle_buffer_550[(uint16_t)rate];
	GenerateTriangleWave((uint16_t)rate, 375, triangle_buffer_375);
	GenerateTriangleWave((uint16_t)rate, 550, triangle_buffer_550);
	// zip buffers
	double zipped_buffer[(uint16_t)rate * 2];
	for (uint16_t i = 0; i < (uint16_t)rate; i++) {
		zipped_buffer[2 * i] = triangle_buffer_375[i];
		zipped_buffer[(2 * i) + 1] = triangle_buffer_550[i];
	}

	uint16_t totalTime = 0;
	for (uint8_t i = 0; i < 100; i++) {
		auto scanStart = daq.getTimeinMicroseconds();
		daq.analogScanOut_all_given_zipped_buffers(zipped_buffer, (uint16_t)rate, true, rate);
		auto scanEnd = daq.getTimeinMicroseconds();
		totalTime += (scanEnd - scanStart);
	}

	cout << "Total time taken: " << totalTime / (100 * 1000) << endl;
}

void hybrid_test() {
	DAQControl daq = DAQControl("20BF9C2");
	cout << "Starting hybrid wave test..." << endl;
	double idealRate = daq.getIdealRate_hybrid(500);
	cout << "Ideal rate according to DAQ: " << idealRate << endl;

	// Plan to use 1 seconds worth of data
	const uint16_t bufferSize = (uint16_t)idealRate;
	double* analogBuffer = new double[bufferSize];
	bool* digitalBuffer = new bool[bufferSize];

	GenerateTriangleWave(bufferSize, 375, analogBuffer);
	GenerateSquareWave(bufferSize, digitalBuffer, 50);

	auto start = daq.getTimeinMicroseconds();
	int numIterations = 100;
	for (int i = 0; i < numIterations; i++) {
		daq.hybridScanOut(0, 0, analogBuffer, digitalBuffer, bufferSize);
	}
	auto end = daq.getTimeinMicroseconds();
	double averageTime = (end - start) / numIterations;

	cout << "Average time for hybrid scan out: " << averageTime << " us" << endl;

}

int main()
{

	DataReciever r("10.10.10.10", 50007);
	while (true) {
		auto data = r.receiveData();
		if (data.size() == 0) {
			continue;
		}
		cout << "Received data with " << data.size() << " rows and " << data[0].size() << " columns." << endl;
	}
	//two_triangles_test();
	//hybrid_test();
	return 0;

}