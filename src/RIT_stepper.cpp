#include "chip.h"

#include "DigitalIoPin.h"
#include "axes.h"
#include "RIT_stepper.h"

int RIT_count;
SemaphoreHandle_t sbRIT;

static DigitalIoPin *toggle_pin;

void RIT_start(void) {
    // RIT status : start
    Chip_RIT_Enable(LPC_RITIMER);
    NVIC_EnableIRQ(RITIMER_IRQn);

    // step 2 : wait for ISR (RIT or PIN_INT) to tell that we're done
    if(xSemaphoreTake(sbRIT, portMAX_DELAY) == pdTRUE) {
        NVIC_DisableIRQ(RITIMER_IRQn);
    }
    else {
        // unexpected error
    }

    // RIT status : stop
}

void RIT_set(int count, int us, DigitalIoPin * step_pin) {
	// step 1 : configure RIT parameters
    RIT_set(count, us);

    // step 2 : write toggle_pin
    toggle_pin = step_pin;
}

void RIT_set(int count, int us) {
    uint64_t cmp_value;

    // step 1 : Determine approximate compare value based on clock rate and passed interval
    cmp_value = (uint64_t) Chip_Clock_GetSystemClockRate() * (uint64_t) us / 1000000;
    
    // step 2 : disable timer during configuration
    Chip_RIT_Disable(LPC_RITIMER);
    
    // step 3 : write toggle count
    RIT_count = count;

    // step 4 : configure RITIMER
    //     enable automatic clear on when compare value==timer value
    //     this makes interrupts trigger periodically
           Chip_RIT_EnableCompClear(LPC_RITIMER);
    //     reset the counter
           Chip_RIT_SetCounter(LPC_RITIMER, 0);
           Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
}

void RIT_stepper_Init(void) {
    // step 1 :
    // initialize RIT (= enable clocking etc.)
    Chip_RIT_Init(LPC_RITIMER);
    // set the priority level of the interrupt
    // The level must be equal or lower than the maximum priority specified in FreeRTOS config
    // Note that in a Cortex-M3 a higher number indicates lower interrupt priority
    NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

    // step 2 : create RIT_stop binary
    sbRIT = xSemaphoreCreateBinary();
}




extern "C"
{
	void RIT_IRQHandler(void) {
		// This used to check if a context switch is required
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		// Tell timer that we have processed the interrupt.
		// Timer then removes the IRQ until next match occurs
		Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
		if(RIT_count > 0) {
			RIT_count--;
			toggle_pin->write(!toggle_pin->read());
			}
			else{
				StopRITFromISR(&xHigherPriorityWoken);
		}
		// End the ISR and (possibly) do a context switch
		portEND_SWITCHING_ISR(xHigherPriorityWoken);
	}
}



