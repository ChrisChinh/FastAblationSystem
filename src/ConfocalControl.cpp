#include "ConfocalControl.h"
#include "../include/CHRocodileLibSpecialFunc.h"
#include "Windows.h"
#include <iostream>
#include <string>
using namespace std;

ConfocalControl::ConfocalControl() {
	this->connect();
}

void ConfocalControl::connect() {
	// Open the connection
	string ip_address = "10.10.10.55";
	Res_t res = OpenConnection(ip_address.c_str(), CHR_2_Device, Connection_Synchronous, 0, &hCHR);
	if (!CHR_SUCCESS(res)) {
		cout << "Failed to connect to confocal" << endl;
		return;
	}

	// Disable the data stream
	Cmd_h hCmd;
	NewCommand(CmdID_Stop_Data_Stream, false, &hCmd);
	ExecCommand(hCHR, hCmd, nullptr);

	// Set the measurement mode to confocal
	SetMeasuringMethod(hCHR, 0, nullptr);

	// set to only detect one peaks
	NewCommand(CmdID_Number_Of_Peaks, false, &hCmd);
	AddCommandIntArg(hCmd, 1);
	ExecCommand(hCHR, hCmd, nullptr);

	// Select optical probe
	NewCommand(CmdID_Optical_Probe, false, &hCmd);
	AddCommandIntArg(hCmd, 0);
	ExecCommand(hCHR, hCmd, nullptr);

	// Set sample rate to 4kHz:
	NewCommand(CmdID_Scan_Rate, false, &hCmd);
	AddCommandFloatArg(hCmd, 4000.0f);
	ExecCommand(hCHR, hCmd, nullptr);

	// Set Averaging to 1 (data written to file):
	NewCommand(CmdID_Data_Average, false, &hCmd);
	AddCommandIntArg(hCmd, 1);
	ExecCommand(hCHR, hCmd, nullptr);

	// Select signals from device
	int32_t an[] = { 83, 16640, 16641 }; // get sample counter, distance and intensity in 16bit integer to save bandwidth
	SetConnectionOutputSignals(hCHR, an, 3, nullptr);

	//Start data stream again
	std::cout << "Parametrized." << std::endl;
}

double ConfocalControl::getDepth() {

	// Set the signals that we are going to read
	const int32_t an[] = { 256 };
	SetConnectionOutputSignals(hCHR, an, 1, nullptr);

	//Begin data stream
	StartDataStream(hCHR, nullptr);

	// Get the depth
	Res_t res;
	const double* pDepth = nullptr;
	int64_t samples = 1;
	TSampleSignalGeneralInfo signalInfo;
	res = GetNextSamples(hCHR, &samples, &pDepth, nullptr, &signalInfo, nullptr);

	// Stop the data stream
	StopDataStream(hCHR, nullptr);

	// Check if we got the depth
	if (!CHR_SUCCESS(res) || samples < 1) {
		cout << "Failed to get depth" << endl;
		return -1;
	}
	cout << "Got " << samples << " samples" << endl;
	FlushConnectionBuffer(hCHR);
	return pDepth[0];
}

void ConfocalControl::reconnect() {
	CloseConnection(hCHR);
	this->connect();
}

ConfocalControl::~ConfocalControl() {
	CloseConnection(hCHR);
}

