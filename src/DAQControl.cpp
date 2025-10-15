#include "DAQControl.h"
#define MAX_DEVICES 100
#define DEFAULT_RANGE BIP10VOLTS

DAQControl::DAQControl(std::string unique_id) {
   uint8_t address;
   char options_str[256];
   convert_options_to_string(OPTIONS, options_str);
}

DAQControl::~DAQControl() {
}

int DAQControl::setAnalogOut(uint8_t channel, float voltage) {
	return 0;
}

int DAQControl::setDigitalOut(uint8_t port_num, bool value) {
	//return cbDBitOut(boardNum, portType, port_num, value);
	return 0;
}

int DAQControl::analogScanOut(uint8_t low_chan, uint8_t high_chan, vector<double> voltages,  bool blocking, double rate) {
	return 0;
}

