#include "dshot/utilities.h"

namespace DShot {
void print_bin(uint16_t var) {
  for (uint32_t test = 0x8000; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
  Serial.println();
}

void print_bin(uint32_t var) {
  for (uint32_t test = 0x80000000; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
}

void print_bin(uint64_t var) {
  for (uint64_t test = 0x8000000000000000; test; test >>= 1) {
    Serial.write(var & test ? '1' : '0');
  }
}

void print_hex(uint8_t x) {
  if (x==0) {
    Serial.print("00");
    return;
  } else if (x<16) {
    Serial.print("0");
  }
  Serial.print(x, HEX);
}

}