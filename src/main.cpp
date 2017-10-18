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

#include "axes.h"
#include "RIT_stepper.h"
#include "pin_interrupt_limits.h"



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

static DigitalIoPin *led;
static void RIT_led(void *pvParameters)
{
	led = new DigitalIoPin(0, 25, DigitalIoPin::output, false);
	axes_Init();

	PIN_INT_Init();
	RIT_stepper_Init();

	dir_x->write(Dir_1);
	dir_y->write(Dir_1);

	led->write(true);
	RIT_set(led, 2 * 200 - 1, 500000);
	RIT_start();
	vTaskDelay(portMAX_DELAY);
}

int main(void)
{
	prvSetupHardware();

	xTaskCreate(RIT_led, "RIT_led", 4 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, NULL);
	
	vTaskStartScheduler();

	while(1);
	return 1;
}
