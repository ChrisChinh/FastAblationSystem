#ifndef DAQ_CONTROL_H
#define DAQ_CONTROL_H

#include <iostream>
#include <stdint.h>

#include "../include/cbw.h"

class DAQControl
{
private:
    // Board number that we assigned to this board.
    int boardNum;

    // Range of the analog voltage.
    int range;

    // Name of the board.
    char boardName[BOARDNAMELEN];

    // DAQ Devuce Descriptor.
    DaqDeviceDescriptor device;

    int portType;

public:
    DAQControl(std::string unique_id, uint8_t board_num);
    ~DAQControl();

    // Set Analog Output voltage.
    int setAnalogOut(uint8_t channel, float voltage);

    // Set Digital Output Status (High / Low).
    int setDigitalOut(uint8_t port_num, bool value);
};

#endif
