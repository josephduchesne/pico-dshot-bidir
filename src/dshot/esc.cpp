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
#include <Arduino.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "pico_pio_loader/pico_pio_loader.h"
#include "dshot/pio/dshot_normal_150.pio.h"
#include "dshot/pio/dshot_normal_300.pio.h"
#include "dshot/pio/dshot_bidir_300.pio.h"
#include "dshot/pio/dshot_normal_600.pio.h"
#include "dshot/pio/dshot_bidir_600.pio.h"
#include "dshot/pio/dshot_normal_1200.pio.h"
#include "dshot/esc.h"
#include "dshot/isr.h"

namespace DShot {

ESC* ESC::sm_to_esc[4] = {nullptr, nullptr, nullptr, nullptr};

bool ESC::init() {
  pio_sm = pio_claim_unused_sm(pio, /*required=*/false);
  if (pio_sm < 0) {
    return false;
  }
  const pio_program_t* dshot_program = nullptr;
  void (*init_dshot_program)(PIO, uint, uint, uint) = nullptr;

  if (speed == Speed::DS150 && type == Type::Normal) {
    dshot_program = &dshot_normal_150_program;
    init_dshot_program = &dshot_normal_150_program_init;

  } else if (speed == Speed::DS300 && type == Type::Normal) {
    dshot_program = &dshot_normal_300_program;
    init_dshot_program = &dshot_normal_300_program_init;

  } else if (speed == Speed::DS300 && type == Type::Bidir) {
    dshot_program = &dshot_bidir_300_program;
    init_dshot_program = &dshot_bidir_300_program_init;
    
  } else if (speed == Speed::DS600 && type == Type::Normal) {
    dshot_program = &dshot_normal_600_program;
    init_dshot_program = &dshot_normal_600_program_init;
    
  } else if (speed == Speed::DS600 && type == Type::Bidir) {
    dshot_program = &dshot_bidir_600_program;
    init_dshot_program = &dshot_bidir_600_program_init;
    
  } else if (speed == Speed::DS1200 && type == Type::Normal) {
    dshot_program = &dshot_normal_1200_program;
    init_dshot_program = &dshot_normal_1200_program_init;
    
  } else {
    // todo: some error about unsupported combos?
    return false;
  }

  if (!pio_loader_add_or_get_offset(pio, dshot_program, &pio_offset)) {
    pio_sm_unclaim(pio, pio_sm);
    pio_sm = -1;
    return false;
  }

  // save a pointer to this lookup
  ESC::sm_to_esc[pio_sm] = this;

  SetupDShotISR();

  // call the init fn
  (*init_dshot_program)(pio, pio_sm, pio_offset, dshot_gpio);
  pio_sm_set_enabled(pio, pio_sm, true);
  return true;
}

uint16_t ESC::setCommand(uint16_t c) {
  pio_sm_put(pio, pio_sm, encoder.encode(c));
  return c;
}

uint16_t ESC::setThrottle(float t) {
  if (t < 0) t = 0;
  if (t > 1) t = 1;

  uint16_t c = MIN_THROTTLE_COMMAND + t * (MAX_THROTTLE_COMMAND - MIN_THROTTLE_COMMAND);
  if (c < MIN_THROTTLE_COMMAND) c = MIN_THROTTLE_COMMAND;
  if (c > MAX_THROTTLE_COMMAND) c = MAX_THROTTLE_COMMAND;
  return setCommand(c);
}

uint16_t ESC::setThrottle3D(float t){ // Set the throttle in range [-1, 1]
  if (t < -1) t = -1;
  if (t > 1) t = 1;

  // todo: zero blanking ms from QueenBee?
  // todo: motor direction?

  uint16_t output = 0;
  if (t == 0) {
    output = 0;
  } else if (t<0) {
    t = -t;
    output = constrain(MID_THROTTLE_COMMAND+(uint16_t)(t*(float)(MAX_THROTTLE_COMMAND-MID_THROTTLE_COMMAND)), MID_THROTTLE_COMMAND, MAX_THROTTLE_COMMAND);
  } else {
    output = constrain(MIN_THROTTLE_COMMAND+(uint16_t)(t*(float)(MID_THROTTLE_COMMAND-1-MIN_THROTTLE_COMMAND)), MIN_THROTTLE_COMMAND, MID_THROTTLE_COMMAND-1);
  }

  return setCommand(output);
}


int ESC::getRawTelemetry(uint64_t& raw_telemetry) {
  if (type == Type::Normal) return false;

  int fifo_words = pio_sm_get_rx_fifo_level(pio, pio_sm);
  if ( fifo_words >= 2) {
    raw_telemetry = (uint64_t)pio_sm_get_blocking(pio, pio_sm) << 32;
    raw_telemetry |= (uint64_t)pio_sm_get_blocking(pio, pio_sm);
    return true;
  } else {
    Serial2.printf("fifow: %d\n", fifo_words);
  }
  return false;
}

void ESC::processTelemetryQueue() {
  RawTelemetry rt;
  uint8_t decoded = 0;
  uint8_t errored = 0;
  while(telemetry_queue.pop(rt)) {
    if (DShot::ESC::sm_to_esc[rt.sm] != nullptr) {
      if (DShot::ESC::sm_to_esc[rt.sm]->decodeTelemetry(rt.telemetry)) {
        DShot::Telemetry& telemetry = DShot::ESC::sm_to_esc[rt.sm]->telemetry;
        
        if (telemetry.reads % 1000 == 0) {
          telemetry.errors = 0;
          telemetry.reads = 0;
        }
        decoded++;
      } else {
        errored++;
      }
    }
  }
}

bool ESC::decodeTelemetry(uint64_t& raw_telemetry) {
  return decoder.decodeTelemetry(raw_telemetry, telemetry);
}

void ESC::setStop() {
  setCommand(0);
}

}  // end namespace DShot