#include "dshot_encoder.h"

#include <stdint.h>

#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/pio.h"

#include "pico_pio_loader/pico_pio_loader.h"

#include "dshot_bidir_300.pio.h"

bool DShotEncoder::init() {
  pio_sm = pio_claim_unused_sm(pio, /*required=*/false);
  if (pio_sm < 0) {
    return false;
  }

  if (!pio_loader_add_or_get_offset(pio, &dshot_bidir_300_program, &pio_offset)) {
    pio_sm_unclaim(pio, pio_sm);
    pio_sm = -1;
    return false;
  }

  dshot_bidir_300_program_init(pio, pio_sm, pio_offset, dshot_gpio);
  pio_sm_set_enabled(pio, pio_sm, true);
  return true;
}

uint16_t DShotEncoder::setCommand(uint16_t c) {
  static bool has_started = false;
  // Shift for telemetry bit (0)
  // if (c < 48) 
  c = (c << 1) | 1;  // migt not be needed
  //else c = (c << 1);
  

  // Shift and include checksum
  uint16_t checksum = (~(c ^ (c >> 4) ^ (c >> 8))) & 0x0F; // slightly different crc for inverted dshot than (c ^ (c >> 4) ^ (c >> 8)) & 0x0F;
  // uint16_t checksum = ((c ^ (c >> 4) ^ (c >> 8))) & 0x0F;
  c = (c << 4) | checksum;

    // maybe pio_sm_clear_fifos here?
    // would need a lock on changing the sm
    // TODO: only enable the first time?
  // pio_sm_restart(pio, pio_sm));

  pio_sm_put(pio, pio_sm, c);
  
  return c;
}

uint16_t DShotEncoder::setThrottle(double t) {
  if (t < 0) t = 0;
  if (t > 1) t = 1;

  uint16_t c = MIN_THROTTLE_COMMAND + t * (MAX_THROTTLE_COMMAND - MIN_THROTTLE_COMMAND);
  if (c < MIN_THROTTLE_COMMAND) c = MIN_THROTTLE_COMMAND;
  if (c > MAX_THROTTLE_COMMAND) c = MAX_THROTTLE_COMMAND;
  return setCommand(c);
}

int DShotEncoder::getTelemetry(uint64_t& raw_telemetry) {
  int fifo_words = pio_sm_get_rx_fifo_level(pio, pio_sm);
    if ( fifo_words >= 2) {
        raw_telemetry = (uint64_t)pio_sm_get_blocking(pio, pio_sm) << 32;
        raw_telemetry |= (uint64_t)pio_sm_get_blocking(pio, pio_sm);
    }
    return fifo_words;
}

void DShotEncoder::stop() {
  pio_sm_set_enabled(pio, pio_sm, false);
}