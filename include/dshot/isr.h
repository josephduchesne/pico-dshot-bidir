#pragma once

#include <Arduino.h>
#include <etl/queue_spsc_locked.h>
#include "dshot/esc.h"

namespace DShot{

class SM_ISR_Control
{
public:
  void lock(){    // Store current interrupt control register and disable the relevant interrupt.  
    //pio_set_irq0_source_enabled(pio0, pis_interrupt0, false);
    hw_clear_bits(&pio0->inte0, PIO_INTR_SM0_BITS | PIO_INTR_SM1_BITS | PIO_INTR_SM2_BITS | PIO_INTR_SM3_BITS);
  }
  void unlock(){    // Restore the interrupt control register.  
    // pio_set_irq0_source_enabled(pio0, pis_interrupt0, true);
    hw_set_bits(&pio0->inte0, PIO_INTR_SM0_BITS | PIO_INTR_SM1_BITS | PIO_INTR_SM2_BITS | PIO_INTR_SM3_BITS);
  }
};

SM_ISR_Control sm_isr_control;
// Create function wrappers with direct calls to the instance's member functions.
etl::function_imv<SM_ISR_Control, sm_isr_control, &SM_ISR_Control::lock> lock;
etl::function_imv<SM_ISR_Control, sm_isr_control, &SM_ISR_Control::unlock> unlock;

typedef struct RawTelemetry {
    uint64_t telemetry;
    uint8_t sm;
} RawTelemetry;

etl::queue_spsc_locked<RawTelemetry, 32> telemetry_queue(lock, unlock);

void on_pio0_irq0() {
    uint64_t raw_telemetry = 0;
    if (pio0_hw->irq & 0b0001) {  // sm0
        raw_telemetry = ((uint64_t)pio_sm_get_blocking(pio0, 0) << 32) | (uint64_t)pio_sm_get_blocking(pio0, 0);
        telemetry_queue.push_from_unlocked({raw_telemetry, 0});
        pio_interrupt_clear (pio0, 0);
    }
    if (pio0_hw->irq & 0b0010) {  // sm1
        raw_telemetry = ((uint64_t)pio_sm_get_blocking(pio0, 1) << 32) | (uint64_t)pio_sm_get_blocking(pio0, 1);
        telemetry_queue.push_from_unlocked({raw_telemetry, 1});
        pio_interrupt_clear (pio0, 1 );
    }
    if (pio0_hw->irq & 0b0100) {  // sm2
        raw_telemetry = ((uint64_t)pio_sm_get_blocking(pio0, 2) << 32) | (uint64_t)pio_sm_get_blocking(pio0, 2);
        telemetry_queue.push_from_unlocked({raw_telemetry, 2});
        pio_interrupt_clear (pio0, 2 );
    }
    if (pio0_hw->irq & 0b1000) {  // sm3
        raw_telemetry = ((uint64_t)pio_sm_get_blocking(pio0, 3) << 32) | (uint64_t)pio_sm_get_blocking(pio0, 3);
        telemetry_queue.push_from_unlocked({raw_telemetry, 3});
        pio_interrupt_clear (pio0, 3 );
    }
}

void SetupDShotISR() {
    static bool has_run = false;
    if (has_run) return;

    // config IRQ for all 4 SMs
    pio_set_irq0_source_enabled(pio0, pis_interrupt0, true);
    pio_set_irq0_source_enabled(pio0, pis_interrupt1, true);
    pio_set_irq0_source_enabled(pio0, pis_interrupt2, true);
    pio_set_irq0_source_enabled(pio0, pis_interrupt3, true);
    irq_set_exclusive_handler(PIO0_IRQ_0, on_pio0_irq0);
    irq_set_enabled(PIO0_IRQ_0, true);

    has_run = true;
}

} // end namespace DShot
