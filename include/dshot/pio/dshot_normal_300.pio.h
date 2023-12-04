// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ---------------- //
// dshot_normal_300 //
// ---------------- //

#define dshot_normal_300_wrap_target 0
#define dshot_normal_300_wrap 17

#define dshot_normal_300_BIT_PERIOD 40

static const uint16_t dshot_normal_300_program_instructions[] = {
            //     .wrap_target
    0xff81, //  0: set    pindirs, 1             [31]
    0x80a0, //  1: pull   block                      
    0x6050, //  2: out    y, 16                      
    0x00e5, //  3: jmp    !osre, 5                   
    0x000d, //  4: jmp    13                         
    0x6041, //  5: out    y, 1                       
    0x006a, //  6: jmp    !y, 10                     
    0xfd01, //  7: set    pins, 1                [29]
    0xe500, //  8: set    pins, 0                [5] 
    0x0003, //  9: jmp    3                          
    0xee01, // 10: set    pins, 1                [14]
    0xf400, // 11: set    pins, 0                [20]
    0x0003, // 12: jmp    3                          
    0xe05a, // 13: set    y, 26                      
    0x1e63, // 14: jmp    !y, 3                  [30]
    0x008e, // 15: jmp    y--, 14                    
    0xb642, // 16: nop                           [22]
    0x0001, // 17: jmp    1                          
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program dshot_normal_300_program = {
    .instructions = dshot_normal_300_program_instructions,
    .length = 18,
    .origin = -1,
};

static inline pio_sm_config dshot_normal_300_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + dshot_normal_300_wrap_target, offset + dshot_normal_300_wrap);
    return c;
}

static inline void dshot_normal_300_program_init(PIO pio, uint sm, uint offset, uint pin) {
    pio_sm_config c = dshot_normal_300_program_get_default_config(offset);
    sm_config_set_set_pins(&c, pin, 1);
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true);
    sm_config_set_out_shift(&c, false, false, 32);   // auto-pull disabled
    double clocks_per_us = clock_get_hz(clk_sys) / 1000000;
    // 3.333us per bit for dshot300
    sm_config_set_clkdiv(&c, 3.333 / dshot_normal_300_BIT_PERIOD * clocks_per_us);
    pio_sm_init(pio, sm, offset, &c);
}

#endif
