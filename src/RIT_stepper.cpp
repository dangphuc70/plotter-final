#include "chip.h"

#include "DigitalIoPin.h"
#include "axes.h"
#include "RIT_stepper.h"

int RIT_count;
SemaphoreHandle_t sbRIT;

static DigitalIoPin *toggle_pin;

void RIT_start() {

}

void RIT_set(int count, int us) {

}

void RIT_stepper_Init() {

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
				give_sbRIT_fromISR(&xHigherPriorityWoken);
		}
		// End the ISR and (possibly) do a context switch
		portEND_SWITCHING_ISR(xHigherPriorityWoken);
	}
}


