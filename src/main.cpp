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

static void axis_test(void *pvParameters){
	DigitalIoPin * sw1   = new DigitalIoPin(0, 17, DigitalIoPin::pullup, true);
	DigitalIoPin * sw3   = new DigitalIoPin(1,  9, DigitalIoPin::pullup, true);
	DigitalIoPin * red   = new DigitalIoPin(0, 25, DigitalIoPin::output);
	DigitalIoPin * green = new DigitalIoPin(0,  3, DigitalIoPin::output);

	Axis led_set(sw1, sw3, red, green, 10, 5);

	green->write(true);

	// driving essential
	rit stepper(green, 1);

	Board_LED_Set(2, false);
	if((led_set -= 5) == 5){
		Board_LED_Set(2, true);
	}

	vTaskDelay(portMAX_DELAY);
}

void task_init(){
	xTaskCreate(axis_test, "axis_test", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

int main(void){
	prvSetupHardware();
	ITM_init();
	
	task_init();

	vTaskStartScheduler();
	return 1;
}
