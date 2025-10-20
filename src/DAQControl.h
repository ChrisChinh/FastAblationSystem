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
#define DEFAULT_SCAN_RATE 18650.0

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

    // Set Analog Output voltages for all channels.
    int setAnalogOut_all(double* voltages);

    // Set Digital Output Status (High / Low).
    int setDigitalOut(uint8_t port_num, bool value);

    // Scan an Analog output
    int analogScanOut(uint8_t low_chan, vector<double> voltages, bool blocking = true, double rate = DEFAULT_SCAN_RATE);

    // Scan an Analog output on both channels
    int analogScanOut_all(vector<double*> voltage_pairs, bool blocking = true, double rate = DEFAULT_SCAN_RATE);

    // Set an Analog output wby passing in a pair of buffers
    int analogScanOut_all_given_two_buffers(double* buffer_1, double* buffer_2, uint16_t buffer_length, bool blocking = true, double rate = DEFAULT_SCAN_RATE);

    int analogScanOut_all_given_two_buffers_zipped(double* buffer_1, double* buffer_2, uint16_t buffer_length, bool blocking = true, double rate = DEFAULT_SCAN_RATE);

    int analogScanOut_all_given_zipped_buffers(double* buffer, uint16_t buffer_length, bool blocking = true, double rate = DEFAULT_SCAN_RATE);
    double getIdealRate(uint16_t num_iterations);
    double getIdealRate_all(uint16_t num_iterations);
};

#endif
