// FastAblationNew.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include "DAQControl.h"
#include "DataReciever.h"
#include <cmath>

using namespace std;

int main()
{
	DAQControl daq = DAQControl("20BF9C2");
	double rate = daq.getIdealRate_all(6000);
	cout << "Ideal rate according to DAQ: " << rate << endl;

	DataReciever r("10.10.10.10", 50007);
	r.sendDouble(rate);


	while (true) {
		auto data = r.receiveData();
		if (data.size() == 0) {
			cout << "No data received, reconnecting..." << endl;
			r.reconnect();
			continue;
		}
		int rows = data.size();
		int cols = data[0].size();
		cout << "Received data with " << data.size() << " rows and " << data[0].size() << " columns." << endl;

		if (rows != 4) {
			cout << "Expected 4 rows of data (for 4 channels), but received " << rows << " rows." << endl;
			continue;
		}

		if (cols < 2) {
			cout << "Expected at least 1 point and header information, but received " << cols << " columns." << endl;
			continue;
		}
		
		int bufferSize = cols;
		int frequency = data[0][0]; // First element is frequency
		for (int i = 1; i < bufferSize; i++) {
			// Unpack the points for each line
			double x1 = data[0][i];
			double y1 = data[1][i];
			double x2 = data[2][i];
			double y2 = data[3][i];
			
			daq.drawLine(x1, y1, x2, y2, frequency, rate);

		}
	}
	return 0;

}