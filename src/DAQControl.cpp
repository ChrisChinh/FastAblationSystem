#include "DAQControl.h"
#define MAX_DEVICES 100
#define DEFAULT_RANGE BIP10VOLTS

DAQControl::DAQControl(std::string unique_id) {
   char options_str[256];
   convert_options_to_string(OPTIONS, options_str);
   select_hat_device(HAT_ID_MCC_152, &address);

   printf("Using HAT device with address %d\n", address);

   int result = mcc152_open(address);
   if (result != RESULT_SUCCESS) {
	   print_error(result);
	   exit(-1);
   }
   result = mcc152_dio_reset(address);
   if (result != RESULT_SUCCESS) {
	   print_error(result);
	   exit(-1);
   }

   for (int channel = 0; channel < mcc152_info()->NUM_DIO_CHANNELS; channel++) {
	   result = mcc152_dio_config_write_bit(address, channel, DIO_DIRECTION, 0);
	   if (result != RESULT_SUCCESS) {
		   print_error(result);
		   exit(-1);
	   }
   }

}

DAQControl::~DAQControl() {
}

uint64_t DAQControl::getTimeinMicroseconds() {
   struct timeval tv;
   gettimeofday(&tv, NULL);
   uint64_t time_in_micros = 1000000 * tv.tv_sec + tv.tv_usec;
   return time_in_micros;
}

int DAQControl::setAnalogOut(uint8_t channel, float voltage) {
	return mcc152_a_out_write(address, channel, OPTIONS, voltage);
}

int DAQControl::setDigitalOut(uint8_t port_num, bool value) {
	return mcc152_dio_output_write_bit(address, port_num, value);
}

int DAQControl::analogScanOut(uint8_t chan, vector<double> voltages, bool blocking, double rate) {
   double waitTime = (1.0 / rate) * 1e6; // in microseconds
   for (auto voltage : voltages) {
      setAnalogOut(chan, voltage);
   }
	return 0;
}

double DAQControl::getIdealRate() {
   setAnalogOut(0, 0);
   double time = getTimeinMicroseconds();
   setAnalogOut(0, 5);
   time = getTimeinMicroseconds() - time;
   return 1e6 / time;
}

