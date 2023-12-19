#pragma once

#include "dshot/drivers/drivers.h"
#include <hardware/pio.h>

namespace DShot::Drivers {

class Pio_SM : public Base
{
public:

    /**
     * @brief Construct a new DShot::Drivers::Pio_SM object
     * 
     * @param dshot_gpio 
     * @param pio 
     * @param type 
     * @param speed 
     */
    Pio_SM(uint dshot_gpio, PIO pio = pio0, Type type = Type::Bidir, Speed speed = Speed::DS300)
    : dshot_gpio(dshot_gpio), pio(pio), speed(speed), type(type)
    {};

    virtual bool init() override;

    virtual uint16_t sendCommand(uint16_t command) override;

    // todo: refactor this somehow
    int getRawTelemetry(uint64_t& raw_telemetry) {
        if (type == Type::Normal) return false;

        int fifo_words = pio_sm_get_rx_fifo_level(pio, pio_sm);
        if ( fifo_words >= 2) {
            raw_telemetry = (uint64_t)pio_sm_get_blocking(pio, pio_sm) << 32;
            raw_telemetry |= (uint64_t)pio_sm_get_blocking(pio, pio_sm);
            return true;
        }
        return false;
    }

    virtual bool getTelemetry(TelemetryField& telemetry) override {

        return true;
    }


private:

    uint dshot_gpio;
    PIO pio;
    uint pio_offset;

    int pio_sm = -1;
    const Speed speed;
    const Type type;

};  // end class DShot::Drivers::Pio

} // end namespace DShot::Drivers