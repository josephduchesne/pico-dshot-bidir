// todo: move PIO specific implementation here, template the backend engine

#include <dshot/drivers/pio_sm.h>
// #include <etl/queue_spsc_atomic.h>
#include <Arduino.h>

// #include "pico/stdlib.h"
#include "hardware/clocks.h"

#include "pico_pio_loader/pico_pio_loader.h"
#include "dshot/drivers/pio/dshot_normal_150.pio.h"
#include "dshot/drivers/pio/dshot_normal_300.pio.h"
#include "dshot/drivers/pio/dshot_bidir_300.pio.h"
#include "dshot/drivers/pio/dshot_normal_600.pio.h"
#include "dshot/drivers/pio/dshot_bidir_600.pio.h"
#include "dshot/drivers/pio/dshot_normal_1200.pio.h"

// etl::queue_spsc_atomic<uint64_t, 16, etl::memory_model::MEMORY_MODEL_SMALL> dshot_queue;

namespace DShot::Drivers {

/**
 * @brief Init the driver, initializing PIO state machine
 * 
 * @return true  Driver ready to use
 * @return false Driver failed to init
 */
bool Pio_SM::init() {
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

    // call the init fn
    (*init_dshot_program)(pio, pio_sm, pio_offset, dshot_gpio);
    pio_sm_set_enabled(pio, pio_sm, true);
    return true;
}

uint16_t Pio_SM::sendCommand(uint16_t command) {
    pio_sm_put(pio, pio_sm, command);
    return command;
}

} // end namespace DShot::Drivers