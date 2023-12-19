/**
 * @file esc.h
 * @author Joseph Duchesne (josephgeek@gmail.com)
 * @author Connor Douthat
 * @brief 
 * @version 0.1
 * @date 2023-12-04
 * 
 * @copyright Copyright (c) 2023 Connor Douthat
 * 
 */

#pragma once

#include <stdint.h>

#include "hardware/pio.h"
#include "dshot/data.h"
#include "dshot/encoder.h"
#include "dshot/decoder.h"
#include "drivers/pio_sm.h"

namespace DShot {
  

// todo: Encoder/Decoder should by parent classes, with type/poles/etc protected
class ESC  {
 public:
  ESC (uint dshot_gpio, PIO pio = pio0, 
               Type type = Type::Bidir, 
               Speed speed = Speed::DS300, 
               unsigned int poles = 14 )
    : dshot_gpio(dshot_gpio), pio(pio), speed(speed), type(type), encoder(type), decoder(poles) {}

  // Todo: move internals to PIOWrapper class: Init PIO, but do not output data yet
  bool init();

  // Set commands trigger once, and must be re-triggered at >= ~200Hz to work
  uint16_t setCommand(uint16_t c); // Set the DShot command value
  // uint16_t setThrottleServo(uint16_t t); // Set thottle in range [1000, 2000] (servo pwm)
  // uint16_t setThrottle3D(double t); // Set the throttle in range [-1, 1]
  // uint16_t setThrottleServo3D(int16_t t); // Throttle range [1000, 2000], 1500 is 0 if ESC is 3d
  uint16_t setThrottle(double t);  // Set the throttle in range [0, 1]
  void setStop();
  
  int getRawTelemetry(uint64_t& raw_telemetry);  // get the currently raw telemetry results, true if there are some
  bool decodeTelemetry(uint64_t& raw_telemetry, Telemetry& telemetry);


  Telemetry telemetry = {0}; // todo: use?

 private:

  // todo: Also move the PIO layer to its own DShot::PIO(gpio, pio, bidir, speed) class
  uint dshot_gpio;
  PIO pio;
  uint pio_offset;

  int pio_sm = -1;
  const Speed speed;
  const Type type;
  Encoder encoder;
  Decoder decoder;
};

} // end namespace DShot
