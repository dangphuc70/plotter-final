#include "chip.h"
#include "DigitalIoPin.h" // for pin initialization

#include "axes.h" // for port and pin numbers of lim_x1, lim_x2, lim_y1, lim_y2, and Direction consts
#include "pin_interrupt_limits.h"
#include "RIT_stepper.h" // for StopRITFromISR ()

// block 1 : Initialize pin interrupt
// for limit switch (4 limits)
void PIN_INT_Init(void)
{
	// Initialize pin-interrupts
	// step 1
	Chip_INMUX_PinIntSel(0, port_lim_x1, pin_lim_x1);
	Chip_INMUX_PinIntSel(1, port_lim_x2, pin_lim_x2);
	Chip_INMUX_PinIntSel(2, port_lim_y1, pin_lim_y1);
	Chip_INMUX_PinIntSel(3, port_lim_y2, pin_lim_y2);

	// step 2 : initialize LPC_GPIO_PIN_INT, enable rising edge interrupt
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	// enable rising edge interrupt
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH1);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH2);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH3);

	// step 3 : set priorities for pin int IRQ handlers
	NVIC_SetPriority(PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT3_IRQn);

}

// block 2 : IRQ Handler for each limits in limit test using ITM indication
extern "C"
{
	// limit x1 IRQ Handler
	void PIN_INT0_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		if(dir_x->read() == Dir_1)
		{
			Chip_RIT_Disable(LPC_RITIMER);
			xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
		}

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
	
	// limit x2 IRQ Handler
	void PIN_INT1_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		if(dir_x->read() == Dir_2)
		{
			Chip_RIT_Disable(LPC_RITIMER);
			xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
		}

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
	
	// limit y1 IRQ Handler
	void PIN_INT2_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		if(dir_y->read() == Dir_1)
		{
			Chip_RIT_Disable(LPC_RITIMER);
			xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
		}

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
	
	// limit y2 IRQ Handler
	void PIN_INT3_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		if(dir_y->read() == Dir_2)
		{
			Chip_RIT_Disable(LPC_RITIMER);
			xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
		}

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
}