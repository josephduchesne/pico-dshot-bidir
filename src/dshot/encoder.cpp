
#include "dshot/encoder.h"
#include <stdint.h>


namespace DShot {

// todo: inline?
// todo: break out checksum for decoder use?
uint16_t Encoder::encode(uint16_t c) {
  // Shift for telemetry bit (0)
  // if (c < 48) 
  c = (c << 1) | 1;  // TODO: might not be needed?
  //else c = (c << 1);
  
  uint16_t checksum;
  if (type == Type::Normal) {
    checksum = ((c ^ (c >> 4) ^ (c >> 8))) & 0x0F;
  } else {
    // slightly different crc for inverted dshot
    checksum = (~(c ^ (c >> 4) ^ (c >> 8))) & 0x0F;
  }
  // Shift and merge 12 bit command with 4 bit checksum
  return  ((c & 0xFFF) << 4) | checksum;
}

}  // end namespace DShot