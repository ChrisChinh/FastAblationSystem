#ifndef DAQ_CONTROL_H
#define DAQ_CONTROL_H

#include <iostream>
#include <stdlib.h>
#include "../include/daqhats_utils.h"
#include <vector>
#include <unistd.h> // For usleep
#include <sys/time.h>

using namespace std;
#define CHANNEL 0
#define OPTIONS OPTS_DEFAULT

#define BUFFER_SIZE 32

class DAQControl
{
private:

    // Name of the board.
    char boardName[8];

    // DAQ Device Descriptor.
    uint8_t address;

    int portType;

public:
    uint64_t getTimeinMicroseconds();

    DAQControl(std::string unique_id);
    ~DAQControl();

    // Set Analog Output voltage.
    int setAnalogOut(uint8_t channel, float voltage);

    // Set Digital Output Status (High / Low).
    int setDigitalOut(uint8_t port_num, bool value);

    // Scan an Analog output
    int analogScanOut(uint8_t low_chan, vector<double> voltages, bool blocking = true, double rate = 5000);

    double getIdealRate();
};

#endif
