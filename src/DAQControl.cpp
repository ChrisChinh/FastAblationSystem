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

int DAQControl::setAnalogOut_all(double* voltages) {
   return mcc152_a_out_write_all(address, OPTIONS, voltages);
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

int DAQControl::analogScanOut_all(vector<double*> voltage_pairs, bool blocking, double rate) {
   double waitTime = (1.0 / rate) * 1e6; // in microseconds
   for (auto voltage_pair : voltage_pairs) {
      setAnalogOut_all(voltage_pair);
   }
   return 0;
}

int DAQControl::analogScanOut_all_given_two_buffers(double* buffer_1, double* buffer_2, uint16_t buffer_length, bool blocking, double rate)
{
   double waitTime = (1.0 / rate) * 1e6; // in microseconds
   // zip the buffers together
   for (uint16_t i = 0; i < buffer_length; i++) {
      double voltages[2] = {buffer_1[i], buffer_2[i]};
      setAnalogOut_all(voltages);
   }
   return 0;
}

int DAQControl::analogScanOut_all_given_two_buffers_zipped(double* buffer_1, double* buffer_2, uint16_t buffer_length, bool blocking, double rate)
{
   double waitTime = (1.0 / rate) * 1e6; // in microseconds
   // zip the buffers together
   double zipped_voltage_tuples[buffer_length * 2];
   for (uint16_t i = 0; i < buffer_length; i++) {
      zipped_voltage_tuples[2 * i] = buffer_1[i];
      zipped_voltage_tuples[(2 * i) + 1] = buffer_2[i];
   }
   for(int i = 0; i < buffer_length; i++){
      mcc152_a_out_write_all(address, OPTIONS, &zipped_voltage_tuples[i * 2]);
   }
   return 0;
}

double DAQControl::getIdealRate(uint16_t num_iterations) {
   setAnalogOut(0, 0);
   double time = getTimeinMicroseconds();
   for (uint16_t i = 0; i < num_iterations; i++) {
      setAnalogOut(0, 5);
      setAnalogOut(0, 0);
   }
   time = getTimeinMicroseconds() - time;
   double avgTime = time /  (2 * num_iterations);
   return (double)1e6 / avgTime;
}

double DAQControl::getIdealRate_all(uint16_t num_iterations) {
   double both_high[2] = {5.0, 5.0};
   double both_low[2] = {0.0, 0.0};
   double time = getTimeinMicroseconds();
   for (uint16_t i = 0; i < num_iterations; i++) {
      setAnalogOut_all(both_high);
      setAnalogOut_all(both_low);
   }
   time = getTimeinMicroseconds() - time;
   double avgTime = time /  (2 * num_iterations);
   return (double)1e6 / avgTime;
}


