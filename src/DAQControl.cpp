#include "DAQControl.h"
#define MAX_DEVICES 100
#define DEFAULT_RANGE BIP10VOLTS

DAQControl::DAQControl(std::string unique_id) {
   chan0Voltage = 0.0;
   chan1Voltage = 0.0;
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
   mcc152_close(address);
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
   chan0Voltage = voltages[0];
   chan1Voltage = voltages[1];
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

int DAQControl::analogScanOut_all_given_zipped_buffers(double* buffer, uint16_t buffer_length, bool blocking, double rate)
{
   double waitTime = (1.0 / rate) * 1e6; // in microseconds
   // zip the buffers together
   for (uint16_t i = 0; i < buffer_length; i += 2) {
      setAnalogOut_all(&buffer[i]);
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

double DAQControl::getIdealRate_hybrid(uint16_t num_iterations) {
   double time = getTimeinMicroseconds();
   for (uint16_t i = 0; i < num_iterations; i++) {
      setAnalogOut(0, 5);
      setDigitalOut(0, true);
      setAnalogOut(0, 0);
      setDigitalOut(0, false);
   }
   time = getTimeinMicroseconds() - time;
   double avgTime = time /  (4 * num_iterations);
   return (double)1e6 / avgTime;
}

bool DAQControl::drawLine(double x1, double y1, double x2, double y2, double speed, double rate) {
   double microns_per_point = speed / rate;
   double length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
   int num_points = length / microns_per_point;
   if (num_points < 2) return false;
   auto x_points = vector<double>(num_points);
   auto y_points = vector<double>(num_points);
   double x_step = (x2 - x1) / (num_points - 1);
   double y_step = (y2 - y1) / (num_points - 1);
   for (int i = 0; i < num_points; i++) {
      x_points[i] = x1 + i * x_step;
      y_points[i] = y1 + i * y_step;
   }
   for (int i = 0; i < num_points; i++) {
      cout << "Setting voltages to (" << x_points[i] << ", " << y_points[i] << ")" << endl;
      double voltages[2] = {x_points[i], y_points[i]};
      setAnalogOut_all(voltages);
   }
   return true;
}

double DAQControl::getVoltage(uint8_t channel) {
   if (channel == 0) {
       return chan0Voltage;
   } else if (channel == 1) {
       return chan1Voltage;
   } else {
       throw std::out_of_range("Invalid channel number");
   }
}