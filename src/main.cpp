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

	Axis * led_set = new Axis(sw1, sw3, red, green, 10000);

	green->write(false);

	// driving essential
	rit stepper(green, 2);

	Board_LED_Set(0, false);
	Board_LED_Set(2, false);
	
	(*led_set) += 5000;

	vTaskDelay(portMAX_DELAY);
}

static void limit_verify_test(void *pvParameters){
	// these 2 lines are ESSENTIAL
	Limit limits(0, 17, 1, 9, 0, 27, 0, 28);
	rit RIT(NULL, 100);

	DigitalIoPin * dir   = new DigitalIoPin(1,  0, DigitalIoPin::output);
	DigitalIoPin * step = new DigitalIoPin(0, 24, DigitalIoPin::output);

	Axis led_set(NULL, NULL, dir, step, 10000);
	
	if(led_set.FindLimit0(limits, 200)){
		
		Board_LED_Set(2, true);
		vTaskDelay(1000);
		Board_LED_Set(2, false);

	}

	Limit::disable();
	led_set += 40;
	Limit::enable();

	if(led_set.FindLimit1(limits, 200)){
		
		Board_LED_Set(2, true);
		vTaskDelay(1000);
		Board_LED_Set(2, false);

	}

	Limit::disable();
	led_set -= 40;
	Limit::enable();

	vTaskDelay(portMAX_DELAY);
}

static void pwm_test(void * pvParameters){
	DigitalIoPin red(0, 25, DigitalIoPin::output);
	PWM large(LPC_SCTLARGE0,
			  0, 25,
			  Chip_Clock_GetSystemClockRate(),
			  Chip_Clock_GetSystemClockRate());
	large = 0.5;
	large.unhalt();

	vTaskDelay(portMAX_DELAY);
}

static void servo_test(void * pvParameters){
	DigitalIoPin red(0, 8, DigitalIoPin::output);
	Servo servo(LPC_SCTLARGE0, 0, 8);
	
	servo.set_duty(Chip_Clock_GetSystemClockRate() / 500);

	vTaskDelay(portMAX_DELAY);
}

static void BresenhamD_test(void * pvParameters){
	rit RIT(NULL, 8000);
	Limit limits(0, 29, 0, 9, 1, 3, 0, 0);

	DigitalIoPin * dir_y = new DigitalIoPin(0, 28, DigitalIoPin::output);
	DigitalIoPin * dir_x = new DigitalIoPin(1, 0, DigitalIoPin::output);
	DigitalIoPin * step_y = new DigitalIoPin(0, 27, DigitalIoPin::output);
	DigitalIoPin * step_x = new DigitalIoPin(0, 24, DigitalIoPin::output);
	
	Axis * x = new Axis(limits[0], limits[1], dir_x, step_x, 10000, 5000);
	Axis * y = new Axis(limits[2], limits[3], dir_y, step_y, 10000, 5000);

	Servo servo(LPC_SCTLARGE0, 0, 10);

	servo = 45;

	servo = 36;

	BresenhamD aline(x, y, 5, 4);
	aline();

	BresenhamD back(x, y, -5, -4);
	back(2);
	vTaskDelay(1000);
	back(3);


	vTaskDelay(portMAX_DELAY);
}

void task_init(){
	xTaskCreate(BresenhamD_test, "BresenhamD_test", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

int main(void){
	prvSetupHardware();
	ITM_init();
	
	task_init();

	vTaskStartScheduler();
	return 1;
}
