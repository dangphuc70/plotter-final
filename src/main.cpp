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

#include "queue.h"
#include "semphr.h"

#include "DigitalIoPin.h"
#include "ITM_write.h"

#include "include.h"


/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, true);
}

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}

static void a1(void *pvParameters){
	DigitalIoPin * sw1 = new DigitalIoPin(0, 17, DigitalIoPin::pullup, true);
	DigitalIoPin * sw3 = new DigitalIoPin(1,  9, DigitalIoPin::pullup, true);
	DigitalIoPin * sw2 = new DigitalIoPin(1, 11, DigitalIoPin::pullup, true);
	DigitalIoPin * led_step = new DigitalIoPin(0, 25, DigitalIoPin::output);
	DigitalIoPin * led_dir = new DigitalIoPin(0,  3, DigitalIoPin::output);

	Axis led_set(sw1, sw3, led_dir, led_step);
}

int main(void)
{
	prvSetupHardware();


	
	vTaskStartScheduler();

	return 1;
}
