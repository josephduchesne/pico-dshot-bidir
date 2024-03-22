#pragma once

#include <stdint.h>

namespace DShot {

    // throttle mapping constants
    static constexpr uint16_t MIN_THROTTLE_COMMAND = 48;
    static constexpr uint16_t MID_THROTTLE_COMMAND = 1048;
    static constexpr uint16_t MAX_THROTTLE_COMMAND = 2047;
    // todo: also handle 3D commands
    // todo: also handle 0 properly as per betaflight post
    // // https://github.com/betaflight/betaflight/issues/2879

    // general math constants
    static constexpr uint32_t ONE_MINUTE_OF_MICROSECONDS = 60000000;

    // dshot specific constants
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_TEMPERATURE = 0x02;
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_VOLTAGE = 0x04;
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_CURRENT = 0x06;
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_DEBUG1 = 0x08;
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_DEBUG2 = 0x0A;
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_STRESS = 0x0C;
    static constexpr uint8_t DSHOT_EXT_TELEMETRY_STATUS = 0x0E;

    enum class Speed {
        DS150 = 0,
        DS300 = 1,
        DS600 = 2,
        DS1200 = 3
    };

    enum class Type {
        Normal = 0,
        Bidir = 1
    };

    typedef struct Telemetry{
        uint32_t eRPM_period_us;  // Micro-seconds per pole-pair phase period
        uint32_t rpm;  // rotations per minute, assuming pole count is correct
        uint8_t temperature_C;  // degrees Celcius
        uint16_t volts_cV;  // deci-volts, or V/100
        uint8_t amps_A;       // Amps
        uint8_t debug1;
        uint8_t debug2;
        uint8_t stress;       // 0..255 see https://github.com/bird-sanctuary/extended-dshot-telemetry/blob/main/README.md
        uint8_t status;       // Bit[7] = alert event, Bit[6] = warning event, Bit[5] = error event, Bit[3-0] - Max. stress level [0-15]
        uint16_t reads;
        uint16_t errors;
    } Telemetry; 

}