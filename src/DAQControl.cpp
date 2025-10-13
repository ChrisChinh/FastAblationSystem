#include "DAQControl.h"
#define CHECK_ERROR(CODE) if((CODE)!=ERR_NO_ERROR)throw CODE


#define MAX_DEVICES 100
#define DEFAULT_RANGE BIP10VOLTS

DAQControl::DAQControl(std::string unique_id) {
    int descriptorIndex = 0;
	DaqDeviceDescriptor devices[MAX_DEVICES];
	DaqDeviceInterface interfaceType = ANY_IFC;
	DaqDeviceHandle handle = 0;
	unsigned int numDevs = MAX_DEVICES;
	Range range = DEFAULT_RANGE;


	CHECK_ERROR(ulGetDaqDeviceInventory(interfaceType, devices, &numDevs));
	if (numDevs == 0) {
		throw "Error: No DAQ Devices found.";
	}

	for (unsigned int i = 0; i < numDevs; i++) {
		DaqDeviceDescriptor dev = devices[i];
		std::cout << "   " << dev.productName << " (" << dev.uniqueId << ") - Device ID = " << dev.productId;
		if (!strcmp(dev.uniqueId, unique_id.c_str())) {
			std::cout << "<-- This one" << std::endl;
			descriptorIndex = i;
		}
		else std::cout << std::endl;
	}

	handle = ulCreateDaqDevice(devices[descriptorIndex]);
	if (handle == 0) {
		throw "Error: Unable to create DAQ device.";
	}

	CHECK_ERROR(ulConnectDaqDevice(handle));
}

DAQControl::~DAQControl() {
	ulDisconnectDaqDevice(handle);
	ulReleaseDaqDevice(handle);
}

int DAQControl::setAnalogOut(uint8_t channel, float voltage) {
	if (voltage > 10.0) voltage = 10.0;
	else if (voltage < -10.0) voltage = -10.0;
	AOutFlag flags = AOUT_FF_DEFAULT;
	int err = ulAOut(handle, channel, range, flags, voltage);
	return err;
}

int DAQControl::setDigitalOut(uint8_t port_num, bool value) {
	//return cbDBitOut(boardNum, portType, port_num, value);
	return 0;
}

int DAQControl::analogScanOut(uint8_t low_chan, uint8_t high_chan, vector<double> voltages, double rate) {
	int num_chans = high_chan - low_chan + 1;
	int samples_per_chan = voltages.size() / num_chans;
	ScanOption scanOptions = ScanOption(SO_DEFAULTIO | SO_CONTINUOUS);
	AOutScanFlag flags = AOUTSCAN_FF_DEFAULT;
	int err = ulAOutScan(handle, low_chan, high_chan, range, samples_per_chan, &rate, scanOptions, flags, voltages.data());
	return err;
}

