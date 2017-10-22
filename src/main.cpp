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

class ChangeDetect
{
	private:

		bool saved;

	public:

		bool operator()(bool b);

	public:

		ChangeDetect(bool saved_ = false);
		~ChangeDetect();
	
};

ChangeDetect::ChangeDetect(bool saved_) : saved(saved_) {}
ChangeDetect::~ChangeDetect() {}

bool ChangeDetect::operator()(bool b){
	if(b != saved){
		saved = b;
		return true;
	}else{
		return false;
	}
}

static void a1(void *pvParameters){

	DigitalIoPin * sw1 = new DigitalIoPin(0, 17, DigitalIoPin::pullup, true);
	DigitalIoPin * sw3 = new DigitalIoPin(1,  9, DigitalIoPin::pullup, true);
	DigitalIoPin * sw2 = new DigitalIoPin(1, 11, DigitalIoPin::pullup, true);
	DigitalIoPin * led_step = new DigitalIoPin(0, 25, DigitalIoPin::output,false);
	DigitalIoPin * led_dir = new DigitalIoPin(0,  3, DigitalIoPin::output, false);

	ChangeDetect changed;

	Axis led_set(sw1, sw3, led_dir, led_step);
	Direction direction_obj = led_set.DirectionObject();
	
	while(1){
		bool sw = sw2->read();
		if(changed(sw)){
			direction_obj(sw);
			int code = direction_obj.show_code();
			char itm[30];
			snprintf(itm, 30, "%d\n", code);
			ITM_write(itm);
		}
	}
}

void task_init(){
	xTaskCreate(a1, "a1", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

int main(void)
{
	prvSetupHardware();
	ITM_init();

	task_init();
	
	vTaskStartScheduler();

	return 1;
}
