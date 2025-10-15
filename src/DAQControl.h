#ifndef DAQ_CONTROL_H
#define DAQ_CONTROL_H

#include <iostream>
#include <stdint.h>
#include <stdlib.h>
#include "../include/daqhats_utils.h"
#include <string.h>
#include <vector>
#include <thread>

using namespace std;
#define CHANNEL 0
#define OPTIONS OPTS_DEFAULT

#define BUFFER_SIZE 32

class DAQControl
{
private:
    // Range of the analog voltage.

    // Name of the board.
    char boardName[8];

    // DAQ Device Descriptor.

    int portType;

public:
    DAQControl(std::string unique_id);
    ~DAQControl();

    // Set Analog Output voltage.
    int setAnalogOut(uint8_t channel, float voltage);

    // Set Digital Output Status (High / Low).
    int setDigitalOut(uint8_t port_num, bool value);

    // Scan an Analog output
    int analogScanOut(uint8_t low_chan, uint8_t high_chan, vector<double> voltages, bool blocking = true, double rate = 5000);
};

#endif
