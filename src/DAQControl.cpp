#include "DAQControl.h"
#include <stdint.h>
#define CHECK_ERROR(CODE) if((CODE)!=NOERRORS)throw CODE

DAQControl::DAQControl(std::string unique_id, uint8_t board_num) {
    /* Variable Initialization */
    boardNum = board_num;
    int ULStat = NOERRORS;
    range = BIP10VOLTS;
	portType = AUXPORT;
	DaqDeviceDescriptor devices[100];
	
	try {
		int number_of_devices = 100;	// Maximum number of devices.
		// Find all DAQ devices.
		ULStat = cbGetDaqDeviceInventory(DaqDeviceInterface::ANY_IFC, devices, &number_of_devices);
        CHECK_ERROR(ULStat);

		if (number_of_devices == 0) {
			throw "Error: No DAQ Devices found.";
		}
		std::cout << "Number of Devices: " << number_of_devices << std::endl;

		// Search for the DAQ with given unique id.
		for (int i = 0; i < number_of_devices; i++) {
			DaqDeviceDescriptor dev = devices[i];
			std::cout << "   " << dev.ProductName <<  " (" << dev.UniqueID << ") - Device ID = " << dev.ProductID;
			if (!strcmp(dev.UniqueID, unique_id.c_str())) {
				std::cout << "<-- This one" << std::endl;
				device = dev;
			}
			else std::cout << std::endl;
		}

		// Create DAQ Device with given board number.
		ULStat = cbCreateDaqDevice(boardNum, device);
        CHECK_ERROR(ULStat);

		// Get name of the board.
		ULStat = cbGetBoardName(boardNum, boardName);
		CHECK_ERROR(ULStat);
            
		printf("Board Name: %s\n\r", boardName);

		// Set Digital port direction.
		ULStat = cbDConfigPort(boardNum, 1, DIGITALOUT);
		CHECK_ERROR(ULStat);
	}
	catch (const int stat) {
		char *msg = NULL;
		cbGetErrMsg(stat, msg);
		std::cout << msg << std::endl;
		cbReleaseDaqDevice(boardNum);
	}
	catch (const char * s) {
		std::cout << s << std::endl;
		cbReleaseDaqDevice(boardNum);
	}
}

DAQControl::~DAQControl() {
    cbReleaseDaqDevice(boardNum);
}

int DAQControl::setAnalogOut(uint8_t channel, float voltage) {
	if (voltage > 10.0) voltage = 10.0;
	else if (voltage < -10.0) voltage = -10.0;
	USHORT dataValue;
	cbFromEngUnits(boardNum, range, voltage, &dataValue);
    return cbAOut(boardNum, channel, range, dataValue);
}

int DAQControl::setDigitalOut(uint8_t port_num, bool value) {
	return cbDBitOut(boardNum, portType, port_num, value);
}
