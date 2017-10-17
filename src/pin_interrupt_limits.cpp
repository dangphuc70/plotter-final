#include "chip.h"

#include "DigitalIoPin.h" // for pin initialization

#include "axes.h"
#include "pin_interrupt_limits.h"
#include "RIT_stepper.h"

// block 1 : Initialize pin interrupt
// for limit switch (4 limits)
void PIN_INT_Init(void)
{
	// Initialize pin-interrupts
	// step 1
	DigitalIoPin LIM_SW_X1(port_lim_x1, pin_lim_x1, DigitalIoPin::pullup, true);
	DigitalIoPin LIM_SW_X2(port_lim_x2, pin_lim_x2, DigitalIoPin::pullup, true);
	DigitalIoPin LIM_SW_Y1(port_lim_y1, pin_lim_y1, DigitalIoPin::pullup, true);
	DigitalIoPin LIM_SW_Y2(port_lim_y2, pin_lim_y2, DigitalIoPin::pullup, true);

	// step 2
	Chip_INMUX_PinIntSel(0, port_lim_x1, pin_lim_x1);
	Chip_INMUX_PinIntSel(1, port_lim_x2, pin_lim_x2);
	Chip_INMUX_PinIntSel(2, port_lim_y1, pin_lim_y1);
	Chip_INMUX_PinIntSel(3, port_lim_y2, pin_lim_y2);

	// step 3
	NVIC_SetPriority(PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	
	// step 4
	NVIC_EnableIRQ  (PIN_INT0_IRQn);
	NVIC_EnableIRQ  (PIN_INT1_IRQn);
	NVIC_EnableIRQ  (PIN_INT2_IRQn);
	NVIC_EnableIRQ  (PIN_INT3_IRQn);
}

extern "C"
{
	void PIN_INT0_IRQHandler(void) {
		
	}
	
	void PIN_INT1_IRQHandler(void) {

	}
	
	void PIN_INT2_IRQHandler(void) {

	}
	
	void PIN_INT3_IRQHandler(void) {

	}
}