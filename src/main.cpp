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

#include "ITM_write.h"
#include "DigitalIoPin.h"


#include "axes.h"
#include "RIT_stepper.h"
#include "pin_interrupt_limits.h"

#include <cstdio> // for snprintf

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, true);
	// initialize RIT (= enable clocking etc.)
	Chip_RIT_Init(LPC_RITIMER);
	// set the priority level of the interrupt
	// The level must be equal or lower than the maximum priority specified in FreeRTOS config
	// Note that in a Cortex-M3 a higher number indicates lower interrupt priority
	NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	Stop_binary_RIT_Init();

}

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}



static DigitalIoPin * led;
static void RIT_led(void *pvParameters)
{
	led = new DigitalIoPin(0, 25, DigitalIoPin::output, false);
	RIT_start_toggle(led, 20, 500000);
	vTaskDelay(portMAX_DELAY);
}

int main(void)
{
	// step 1 : initialize hardware
	prvSetupHardware();
	ITM_init();

	xTaskCreate(RIT_led, "RIT_led", 4 * configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1UL, NULL);
	
	vTaskStartScheduler();
	return 1;
}
