#ifndef DAQ_CONTROL_H
#define DAQ_CONTROL_H

#include <iostream>
#include <stdlib.h>
#include "../include/daqhats_utils.h"
#include <vector>
#include <unistd.h> // For usleep
#include <sys/time.h>
#include <cmath>
#include <thread>
#include <atomic>

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

    double chan0Voltage, chan1Voltage;

    double chan0Bias, chan1Bias;

    // Scan an Analog output on both channels
    int analogScanOut_all(vector<double*> voltage_pairs, bool blocking = true, double rate = DEFAULT_SCAN_RATE);

    // Generate triangle wave
    vector<double> triangleWaveBuffer(int rate, double frequency, double amplitude);
    void triangleWave(uint8_t channel, double amplitude, double frequency, double rate);

    // Atomics for triangle wave thread
    atomic<bool> triangleLoopStop_{false};
    atomic<bool> triangleLoopRunning_{false};
    thread triangleThread_;


public:
    uint64_t getTimeinMicroseconds();

    DAQControl(uint8_t addr = 0);
    ~DAQControl();

    // Set Analog Output voltage.
    int setAnalogOut(uint8_t channel, float voltage);

    // Set Analog Output voltages for all channels.
    int setAnalogOut_all(double* voltages);

    // Set Digital Output Status (High / Low).
    int setDigitalOut(uint8_t port_num, bool value);

    // Scan an Analog output
    int analogScanOut(uint8_t low_chan, vector<double> voltages, bool blocking = true, double rate = DEFAULT_SCAN_RATE);

    bool drawLine(double x1, double y1, double x2, double y2, double speed, double rate);

    double getIdealRate(uint16_t num_iterations);

    // Get current position of galvos
    double getVoltage(uint8_t channel);

    void setBias(uint8_t channel, double bias);

    // Triangle wave
    void startTriangleLoop(uint8_t channel, double amplitude, double frequency, double rate);
    void stopTriangleLoop();
    bool isTriangleLoopRunning() const;

};

#endif
