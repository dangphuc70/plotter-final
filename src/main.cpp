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

void rit_test(int port,
			  int pin,
			  DigitalIoPin::pinMode mode,
			  bool invert,
			  int count,
			  int pps){
	DigitalIoPin * opin = new DigitalIoPin(port, pin, mode, invert);
	rit stepper(opin, pps, count);
	stepper.WritePin(true);
	stepper.Run();
	delete opin;
}

static void rit_test(void *pvParameters){
	rit_test(0, 25, DigitalIoPin::output, false, 2*10-1, 4);
	vTaskDelay(portMAX_DELAY);
}

void task_init(){
	xTaskCreate(rit_test, "rit_test", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

int main(void){
	prvSetupHardware();
	ITM_init();
	
	task_init();

	vTaskStartScheduler();
	return 1;
}
