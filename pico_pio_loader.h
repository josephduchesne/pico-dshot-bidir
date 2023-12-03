#ifndef __PICO_PIO_LOADER_PICO_PIO_LOADER_H__
#define __PICO_PIO_LOADER_PICO_PIO_LOADER_H__

#include "hardware/pio.h"

// Note: not thread safe!
bool pio_loader_add_or_get_offset(PIO pio, const pio_program_t* program, uint* offset);

#endif