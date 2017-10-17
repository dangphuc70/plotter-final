/*
===============================================================================
 Name        : main.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
===============================================================================
*/

#if defined (__USE_LPCOPEN)
#if defined(NO_BOARD_LIB)
#include "chip.h"
#else
#include "board.h"
#endif
#endif


#include "FreeRTOS.h"
#include "task.h"
#include "DigitalIoPin.h"
#include "queue.h"
#include "semphr.h"

#include "DigitalIoPin.h"
#include "ITM_write.h"

#include "axes.h"
#include "RIT_stepper.h"
#include <pin_interrupt_limits.h>



/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, true);
}

int main(void)
{
	step_x->write(true);

	while(1)
	{

	}
	return 1;
}
