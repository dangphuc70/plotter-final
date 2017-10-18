#include "RIT_stepper.h"

// RIT Setup
volatile uint32_t RIT_count;
static DigitalIoPin *RIT_toggle_pin;

xSemaphoreHandle sbRIT;
void Stop_binary_RIT_Init(void)
{
    sbRIT = xSemaphoreCreateBinary();
}

extern "C" {
void RIT_IRQHandler(void)
{
    // This used to check if a context switch is required
    portBASE_TYPE xHigherPriorityWoken = pdFALSE;
    // Tell timer that we have processed the interrupt.
    // Timer then removes the IRQ until next match occurs
    Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
    if(RIT_count > 0) {
        RIT_count--;
        RIT_toggle_pin->write( !RIT_toggle_pin->read() );
    }

    if(RIT_count <= 0) {
        Chip_RIT_Disable(LPC_RITIMER); // disable timer
        // Give semaphore and set context switch flag if a higher priority task was woken up
        xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
    }
    // End the ISR and (possibly) do a context switch
    portEND_SWITCHING_ISR(xHigherPriorityWoken);
}
}

void RIT_start_toggle(DigitalIoPin *step_pin, int count, int us)
{
    uint64_t cmp_value;
    // Determine approximate compare value based on clock rate and passed interval
    cmp_value = (uint64_t) Chip_Clock_GetSystemClockRate() * (uint64_t) us / 1000000;
    // disable timer during configuration
    Chip_RIT_Disable(LPC_RITIMER);
    RIT_count = count;
    RIT_toggle_pin = step_pin;
    // enable automatic clear on when compare value==timer value
    // this makes interrupts trigger periodically
    Chip_RIT_EnableCompClear(LPC_RITIMER);
    // reset the counter
    Chip_RIT_SetCounter(LPC_RITIMER, 0);
    Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
    // start counting
    Chip_RIT_Enable(LPC_RITIMER);
    // Enable the interrupt signal in NVIC (the interrupt controller)
    NVIC_EnableIRQ(RITIMER_IRQn);
    // wait for ISR to tell that we're done
    if(xSemaphoreTake(sbRIT, portMAX_DELAY) == pdTRUE) {
        // Disable the interrupt signal in NVIC (the interrupt controller)
        NVIC_DisableIRQ(RITIMER_IRQn);
    }
    else {
    // unexpected error
    }
}


