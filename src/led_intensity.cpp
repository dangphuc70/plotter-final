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
#include <cstdio> // for debug messages
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
}

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}

#define boolDIRECTION_2 true
#define boolDIRECTION_1 false

bool step_OK(bool dir, bool l1, bool l2){
	if((dir == boolDIRECTION_1) && l1) return false;
	else if((dir == boolDIRECTION_2) && l2) return false;
	else return true;
}

bool step_OK(DigitalIoPin *DIR, DigitalIoPin *LIM1, DigitalIoPin *LIM2){
	return step_OK(DIR->read(), LIM1->read(), LIM2->read());
}


// RIT Setup
volatile uint32_t RIT_count;
static DigitalIoPin *RIT_toggle_pin;

static xSemaphoreHandle sbRIT;
void Stop_binary_RIT_Init(void)
{
	sbRIT = xSemaphoreCreateBinary();
}

extern "C" {
void RIT_IRQHandler(void)
{
	// This used to check if a context switch is required
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	// Tell timer that we have processed the interrupt.
	// Timer then removes the IRQ until next match occurs
	Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
	if(RIT_count > 0) {
		RIT_count--;
		RIT_toggle_pin->write( !RIT_toggle_pin->read() );
	}

	if(RIT_count <= 0) {
		Chip_RIT_Disable(LPC_RITIMER); // disable timer
		// Give semaphore and set context switch flag if a higher priority task was woken up
		xSemaphoreGiveFromISR(sbRIT, &xHigherPriorityWoken);
	}
	// End the ISR and (possibly) do a context switch
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}
}

void RIT_start_toggle(DigitalIoPin *step_pin, int count, int us)
{
	uint64_t cmp_value;
	// Determine approximate compare value based on clock rate and passed interval
	cmp_value = (uint64_t) Chip_Clock_GetSystemClockRate() * (uint64_t) us / 1000000;
	// disable timer during configuration
	Chip_RIT_Disable(LPC_RITIMER);
	RIT_count = count;
	RIT_toggle_pin = step_pin;
	// enable automatic clear on when compare value==timer value
	// this makes interrupts trigger periodically
	Chip_RIT_EnableCompClear(LPC_RITIMER);
	// reset the counter
	Chip_RIT_SetCounter(LPC_RITIMER, 0);
	Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
	// start counting
	Chip_RIT_Enable(LPC_RITIMER);
	// Enable the interrupt signal in NVIC (the interrupt controller)
	NVIC_EnableIRQ(RITIMER_IRQn);
	// wait for ISR to tell that we're done
	if(xSemaphoreTake(sbRIT, portMAX_DELAY) == pdTRUE) {
		// Disable the interrupt signal in NVIC (the interrupt controller)
		NVIC_DisableIRQ(RITIMER_IRQn);
	}
	else {
	// unexpected error
	}
}


void step(DigitalIoPin *STEP, int us){
	// step 1 : set STEP to false
	STEP->write(false);
	// step 2 : start the RIT for the RIT_ISR to toggle the STEP pin
	RIT_start_toggle(STEP, 2, us);
}

typedef struct StepData {
	DigitalIoPin *STEP;
	DigitalIoPin *DIR;
	DigitalIoPin *LIM1;
	DigitalIoPin *LIM2;
	int demand;
	bool direction;
	int us;
} StepData;

static StepData *on_data;
static SemaphoreHandle_t On_data_mutex;

void On_data_Init(void)
{
	on_data = new StepData;
	On_data_mutex = xSemaphoreCreateMutex();
}

enum StepEnum {
	eConstraint,
	eComplete,
	eNotComplete
};

StepEnum step_on_data_STATUS(){
	StepEnum result;
	if(xSemaphoreTake(On_data_mutex, portMAX_DELAY)){
		if(on_data->demand == 0)
			result = eComplete;
		else if(step_OK( on_data->DIR->read(), on_data->LIM1->read(), on_data->LIM2->read() )){
			// step 1 : step
			step(on_data->STEP, on_data->us);
			// step 2 : decrease demand
			on_data->demand --;
			// step 3 : check demand, see if step taken was the last step
			if(on_data->demand > 0)
				result = eNotComplete;
			else if(on_data->demand == 0)
				result = eComplete;
			else
				result = eComplete; // for safety
		}
		else{
			result = eConstraint;
		}

		xSemaphoreGive(On_data_mutex);
	}
	return result;
}




// STEP TASKS RELATED
static SemaphoreHandle_t Constraint_binary;
static SemaphoreHandle_t StepGO_binary;
static SemaphoreHandle_t Step_stopped_binary;
void step_task(void*);
//void constraint_resolve_task(void*);

void step_tasks_Init(void)
{
	// Constraint_binary = xSemaphoreCreateBinary();
	StepGO_binary = xSemaphoreCreateBinary();
	Step_stopped_binary = xSemaphoreCreateBinary();

	xTaskCreate(step_task, "step_task", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1UL, NULL);
	//xTaskCreate(constraint_resolve_task, "constraint_resolve_task", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

void step_set_on_data(DigitalIoPin *step_pin,
					  DigitalIoPin *dir_pin,
					  DigitalIoPin *lim1,
					  DigitalIoPin *lim2,
					  int demand,
					  bool direction,
					  int us){
	if(xSemaphoreTake(On_data_mutex, portMAX_DELAY) == pdTRUE){
		on_data->STEP = step_pin;
		on_data->DIR = dir_pin;
		on_data->LIM1 = lim1;
		on_data->LIM2 = lim2;
		on_data->demand = demand;
		on_data->DIR->write(direction);
		on_data->us = us;
		xSemaphoreGive(On_data_mutex);
	}
}



void step_task(void *pvParameters){

	StepEnum status;
	// step 1 : wait for semaphore
	while(1){
		if(xSemaphoreTake(StepGO_binary, portMAX_DELAY) == pdTRUE){
			while(1)
			{
				status = step_on_data_STATUS();
				if(status == eComplete){
					xSemaphoreGive(Step_stopped_binary);
					break;
				}
				else if(status == eConstraint){
					xSemaphoreGive(Step_stopped_binary);
					// xSemaphoreGive(Constraint_binary);
					break;
				}
			}
		}
	}
}

// void constrait_resolve_on_data(){
// 	if(xSemaphoreTake(On_data_mutex, portMAX_DELAY) == pdTRUE){
// 		on_data->demand = 0;
// 		xSemaphoreGive(On_data_mutex);
// 	}
// }

// void constraint_resolve_task(void *pvParameters){
// 	while(1){
// 		if(xSemaphoreTake(Constraint_binary, portMAX_DELAY) == pdTRUE){
// 			constrait_resolve_on_data();
// 		}
// 	}
// }

static DigitalIoPin* STEPX;
static DigitalIoPin* DIRX;
static DigitalIoPin* LIM1X;
static DigitalIoPin* LIM2X;

void X_axis_Init(void){
	STEPX = new DigitalIoPin(0, 24, DigitalIoPin::output, false);
	DIRX = new DigitalIoPin(1, 0, DigitalIoPin::output, false);
	LIM1X = new DigitalIoPin(0, 29, DigitalIoPin::pullup, true);
	LIM2X = new DigitalIoPin(0, 9, DigitalIoPin::pullup, true);
}

static DigitalIoPin* STEPY;
static DigitalIoPin* DIRY;
static DigitalIoPin* LIM1Y;
static DigitalIoPin* LIM2Y;

void Y_axis_Init(void){
	STEPY = new DigitalIoPin(0, 27, DigitalIoPin::output, false);
	DIRY = new DigitalIoPin(0, 28, DigitalIoPin::output, false);
	LIM1Y = new DigitalIoPin(1, 3, DigitalIoPin::pullup, true);
	LIM2Y = new DigitalIoPin(0, 0, DigitalIoPin::pullup, true);
}

bool test_switch(DigitalIoPin *sw){
	if(sw->read()){
		while(sw->read());
		return true;
	}
	else{
		return false;
	}
}

// void limit_test(void *pvParameters){
// 	DigitalIoPin limx1 (0, 29, DigitalIoPin::pullup, true);
// 	DigitalIoPin limx2 (0, 9, DigitalIoPin::pullup, true);
// 	DigitalIoPin limy1 (1, 3, DigitalIoPin::pullup, true);
// 	DigitalIoPin limy2 (0, 0, DigitalIoPin::pullup, true);


// 	while(1){
// 		if(test_switch(&limx1)) ITM_write("X 1\n");
// 		if(test_switch(&limx2)) ITM_write("X 2\n");
// 		if(test_switch(&limy1)) ITM_write("Y 1\n");
// 		if(test_switch(&limy2)) ITM_write("Y 2\n");
// 		vTaskDelay(configTICK_RATE_HZ/1000);
// 	}
// }

void StepGO(){
	xSemaphoreGive(StepGO_binary);
}

void WaitStepStopped(){
	xSemaphoreTake(Step_stopped_binary, portMAX_DELAY);
}

void test_line_task(void *pvParameters){
	X_axis_Init();
	Y_axis_Init();

	step_set_on_data(STEPY,
					 DIRY,
					 LIM1Y,
					 LIM2Y,
					 2000,
					 boolDIRECTION_2,
					 2000);
	StepGO();
	WaitStepStopped();
	step_set_on_data(STEPX,
					 DIRX,
					 LIM1X,
					 LIM2X,
					 2000,
					 boolDIRECTION_1,
					 2000);	
	StepGO();
	WaitStepStopped();

}

void GoHome(){
	DIRX->write(boolDIRECTION_1);
	DIRY->write(boolDIRECTION_1);
	while(step_OK(DIRX, LIM1X, LIM2X)){
		step(STEPX, 6);
	}
	while(step_OK(DIRY, LIM1Y, LIM2Y)){
		step(STEPY, 6);
	}
}

void GoHome_test(void *pvParameters){
	X_axis_Init();
	Y_axis_Init();
	GoHome();
	vTaskDelay(portMAX_DELAY);
}

void test_init()
{
	xTaskCreate(GoHome_test, "GoHome_test", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

int main(void) {

	prvSetupHardware();
	ITM_init();
	Stop_binary_RIT_Init();

	On_data_Init();
	step_tasks_Init();

	test_init();
//	xTaskCreate(limit_test, "limit_test", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1UL, NULL);



	vTaskStartScheduler();
	while(1);
    return 1 ;
}
