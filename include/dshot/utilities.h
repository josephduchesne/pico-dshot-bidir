#pragma once

#include <stdint.h>
// todo: ifdef out all the arduino stuff
#include <Arduino.h>

namespace DShot {

void print_bin(uint16_t var);
void print_bin(uint32_t var);
void print_bin(uint64_t var);
void print_hex(uint8_t x);

} // end namespace DShot