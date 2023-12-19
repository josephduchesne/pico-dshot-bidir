/**
 * @file esc.cpp
 * @author Joseph Duchesne (josephgeek@gmail.com)
 * @author Connor Douthat
 * @brief 
 * @version 0.1
 * @date 2023-12-04
 * 
 * @copyright Copyright (c) 2023 Connor Douthat
 * 
 */

#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "pico_pio_loader/pico_pio_loader.h"
#include "dshot/drivers/pio/dshot_normal_150.pio.h"
#include "dshot/drivers/pio/dshot_normal_300.pio.h"
#include "dshot/drivers/pio/dshot_bidir_300.pio.h"
#include "dshot/drivers/pio/dshot_normal_600.pio.h"
#include "dshot/drivers/pio/dshot_bidir_600.pio.h"
#include "dshot/drivers/pio/dshot_normal_1200.pio.h"
#include "dshot/esc.h"

namespace DShot {

bool ESC::init() {
  return driver.init();
}

uint16_t ESC::setCommand(uint16_t c) {
  return driver.sendCommand(encoder.encode(c));
}

uint16_t ESC::setThrottle(double t) {
  if (t < 0) t = 0;
  if (t > 1) t = 1;

  uint16_t c = MIN_THROTTLE_COMMAND + t * (MAX_THROTTLE_COMMAND - MIN_THROTTLE_COMMAND);
  if (c < MIN_THROTTLE_COMMAND) c = MIN_THROTTLE_COMMAND;
  if (c > MAX_THROTTLE_COMMAND) c = MAX_THROTTLE_COMMAND;
  return setCommand(c);
}

int ESC::getRawTelemetry(uint64_t& raw_telemetry) {
  return driver.getRawTelemetry(raw_telemetry);
}

bool ESC::decodeTelemetry(uint64_t& raw_telemetry, Telemetry& telemetry) {
  return decoder.decodeTelemetry(raw_telemetry, telemetry);
}

void ESC::setStop() {
  setCommand(0);
}

}  // end namespace DShot