#ifndef __PICO_DSHOT_DSHOT_ENCODER_H__
#define __PICO_DSHOT_DSHOT_ENCODER_H__

#include <stdint.h>

#include "hardware/pio.h"


typedef struct DShotTelemetry{
    uint32_t eRPM_period_us;  // Micro-seconds per pole-pair phase period
    uint32_t rpm;  // rotations per minute, assuming pole count is correct
    uint8_t temperature_C;  // degrees Celcius
    uint16_t volts_cV;  // deci-volts, or V/100
    uint8_t amps_A;       // Amps
    uint16_t reads;
    uint16_t errors;
} DShotTelemetry; 

class DShotEncoder {
 public:
  DShotEncoder(uint dshot_gpio, PIO pio = pio0)
    : dshot_gpio(dshot_gpio), pio(pio) {}
  // TODO: cleanup in destructor

  // Init PIO, but do not output data yet
  bool init();

  // Set methods begin continuously generating output, repeating the last value
  uint16_t setCommand(uint16_t c); // Set the DShot command value
  uint16_t setThrottle(double t);  // Set the throttle in range [0, 1]
  int getTelemetry(uint64_t& raw_telemetry);  // get the currently raw telemetry results, true if there are some

  // Stop generating output (until next set command)
  void stop();

 private:
  static constexpr uint16_t MIN_THROTTLE_COMMAND = 48;
  static constexpr uint16_t MAX_THROTTLE_COMMAND = 2047;

  uint dshot_gpio;

  PIO pio;
  uint pio_offset;
  int pio_sm = -1;
};

#endif