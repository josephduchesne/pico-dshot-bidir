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

namespace DShot {
  

// todo: Encoder/Decoder should by parent classes, with type/poles/etc protected
class ESC  {
 public:
 /**
  * @brief Construct a new ESC object
  * 
  * @param dshot_gpio 
  * @param pio 
  * @param type 
  * @param speed 
  * @param poles    Default 14. Allows calculation of RPM from eRPM.
  * @param scale    Set the max scale (default 1.0)/ 0.5 will treat 100% throttle input as 50% throttle output
  * @param reversed Reverse the default direction (only affects setThrottle3D)
  */
  ESC (uint dshot_gpio, PIO pio = pio0, 
               Type type = Type::Bidir, 
               Speed speed = Speed::DS300, 
               unsigned int poles = 14, float scale = 1.0, bool reversed = false )
    : dshot_gpio(dshot_gpio), pio(pio), speed(speed), type(type), encoder(type), decoder(poles), scale(scale), reversed(reversed) {}

  // Todo: move internals to PIOWrapper class: Init PIO, but do not output data yet
  bool init();

  // Set commands trigger once, and must be re-triggered at >= ~200Hz to work
  uint16_t setCommand(uint16_t c); // Set the DShot command value
  // uint16_t setThrottleServo(uint16_t t); // Set thottle in range [1000, 2000] (servo pwm)

  static uint16_t convertThrottle3D(float t); // Convert throttle in range [-1, 1] where 0 is stopped to dshot
  uint16_t setThrottle3D(float t); // Set the throttle in range [-1, 1]
  // uint16_t setThrottleServo3D(int16_t t); // Throttle range [1000, 2000], 1500 is 0 if ESC is 3d
  uint16_t setThrottle(float t);  // Set the throttle in range [0, 1]
  void setStop();
  
  int getRawTelemetry(uint64_t& raw_telemetry);  // get the currently raw telemetry results, true if there are some
  bool decodeTelemetry(uint64_t& raw_telemetry);

  static void processTelemetryQueue();

  static ESC* sm_to_esc[4];
  Telemetry telemetry = {0}; // todo: use?
  int pio_sm = -1;
  uint16_t output = 0;
 private:

  // todo: Also move the PIO layer to its own DShot::PIO(gpio, pio, bidir, speed) class
  uint dshot_gpio;
  PIO pio;
  uint pio_offset;

  const bool reversed;
  const float scale;
  const Speed speed;
  const Type type;
  Encoder encoder;
  Decoder decoder;
};

} // end namespace DShot
