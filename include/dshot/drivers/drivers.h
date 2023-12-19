#pragma once

#include "dshot/data.h"
// #include <etl/observer.h>

namespace DShot::Drivers {

/**
 * @brief Constats used by all/most dshot drivers
 * 
 */
inline constexpr uint16_t TELEMETRY_QUEUE_MAX = 16;
inline constexpr uint16_t MAX_OBSERVERS = 1;

// typedef etl::observer<const DShot::TelemetryField&> Telemetry_Observer;

typedef int TelemetryField;  // temporary

class Base // : public etl::observable<Telemetry_Observer, MAX_OBSERVERS>
{
public:

    virtual bool init() = 0;

    virtual uint16_t sendCommand(uint16_t command) = 0;

    virtual bool getTelemetry(TelemetryField& telemetry) = 0;


};

}  // end namespace DShot::Drivers