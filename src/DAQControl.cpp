#include "DAQControl.h"
#define MAX_DEVICES 100
#define DEFAULT_RANGE BIP10VOLTS

DAQControl::DAQControl() {
   chan0Voltage = 0.0;
   chan1Voltage = 0.0;
   chan0Bias = 0.0;
   chan1Bias = 0.0;
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
   if (channel == 0) {
       voltage = voltage + chan0Bias;
       chan0Voltage = voltage;
   } else if (channel == 1) {
       voltage = voltage + chan1Bias;
       chan1Voltage = voltage; 
   } else {
       throw std::out_of_range("Invalid channel number");
   }
	return mcc152_a_out_write(address, channel, OPTIONS, voltage);
}

int DAQControl::setAnalogOut_all(double* voltages) {
   voltages[0] += chan0Bias;
   voltages[1] += chan1Bias;
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

double DAQControl::getIdealRate(uint16_t num_iterations) {
   setDigitalOut(0, false);
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

bool DAQControl::drawLine(double x1, double y1, double x2, double y2, double speed, double rate) {
   double v_per_point = speed / rate;
   double length = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
   int num_points = length / v_per_point;
   if (num_points < 2) {
      double voltages[2] = {x2, y2};
      setAnalogOut_all(voltages);
      return false;
   }
   auto x_points = vector<double>(num_points);
   auto y_points = vector<double>(num_points);
   double x_step = (x2 - x1) / (num_points - 1);
   double y_step = (y2 - y1) / (num_points - 1);
   for (int i = 0; i < num_points; i++) {
      x_points[i] = x1 + i * x_step;
      y_points[i] = y1 + i * y_step;
   }
   for (int i = 0; i < num_points; i++) {
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

void DAQControl::setBias(uint8_t channel, double bias) {
   if (channel == 0) {
       chan0Bias = bias;
   } else if (channel == 1) {
       chan1Bias = bias;
   } else {
       throw std::out_of_range("Invalid channel number");
   }
}

void DAQControl::triangleWave(uint8_t channel, double amplitude, double frequency, double rate) {

   vector<double> buffer = triangleWaveBuffer((int)rate, frequency, amplitude);
   analogScanOut(channel, buffer, false, rate);

}

vector<double> DAQControl::triangleWaveBuffer(int rate, double frequency, double amplitude) {
   vector<double> buffer(rate);
   double min = 0.0;
   double max = amplitude;
   double range = max - min;
   double samplesPerPeriod = rate / frequency;

   for (int i = 0; i < rate; i++) {
      int phase_index = i % (int)samplesPerPeriod;
      if (phase_index < samplesPerPeriod / 2) {
         buffer[i] = min + (phase_index * range / (samplesPerPeriod / 2));
      } else {
         buffer[i] = max - ((phase_index - samplesPerPeriod / 2) * range / (samplesPerPeriod / 2));
      }
   }

   return buffer;
}

void DAQControl::startTriangleLoop(uint8_t channel, double amplitude, double frequency, double rate) {
   if (triangleLoopRunning_) {
      return;
   }
   triangleLoopStop_ = false;
   triangleLoopRunning_ = true;
   triangleThread_ = thread([=]() {
      while (!triangleLoopStop_) {
         triangleWave(channel, amplitude, frequency, rate);
         this_thread::yield();
      }
      triangleLoopRunning_ = false;
   });
}

void DAQControl::stopTriangleLoop() {
   if (!triangleLoopRunning_) {
      return;
   }
   triangleLoopStop_ = true;
   if (triangleThread_.joinable()) {
      triangleThread_.join();
   }
}

bool DAQControl::isTriangleLoopRunning() const {
   return triangleLoopRunning_;
}