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
#include "event_groups.h"

EventGroupHandle_t io_sync;
EventGroupHandle_t cdc_sync;

QueueHandle_t xQueue;
Fmutex *mtx;
TaskHandle_t xTaskCDCHandle;

/* Sets up system hardware */
static void prvSetupHardware(void)
{
	SystemCoreClockUpdate();
	Board_Init();

	/* Initial LED0 state is off */
	Board_LED_Set(0, true);

	xQueue = xQueueCreate( 10, sizeof( UniversalClass::task ) );
	mtx = new Fmutex();
	xTaskCDCHandle = NULL;

	io_sync = xEventGroupCreate();
	cdc_sync = xEventGroupCreate();
}

/* the following is required if runtime statistics are to be collected */
extern "C" {

void vConfigureTimerForRunTimeStats( void ) {
	Chip_SCT_Init(LPC_SCTSMALL1);
	LPC_SCTSMALL1->CONFIG = SCT_CONFIG_32BIT_COUNTER;
	LPC_SCTSMALL1->CTRL_U = SCT_CTRL_PRE_L(255) | SCT_CTRL_CLRCTR_L; // set prescaler to 256 (255 + 1), and start timer
}

}

static void receive_task(void *pvParameters)
{
	xEventGroupWaitBits( cdc_sync,
						 (1 << 0),
						 pdFALSE,
						 pdTRUE,
						 portMAX_DELAY);
	vTaskPrioritySet(xTaskCDCHandle,(tskIDLE_PRIORITY + 1UL));
	UniversalClass::Task t;
	GCodeInterpreter GCI;
	uint32_t len;
	char c[40];

	xEventGroupSetBits( io_sync,
						(1 << 0) );
	while(true)
	{
		len = USB_receive((uint8_t *)c, 39);
		if (len > 0)
		{
			c[len] = 0;
			ITM_write(c);
			t = GCI.CreateTask(c,len); // But it does not check if task is correct.
			xQueueSendToBack( xQueue, &t,  portMAX_DELAY);
			GCI.sendOK();
		}
	}
}

static void task(void *pvParameters){

	xEventGroupWaitBits( io_sync,
						 (1 << 0),
						 pdFALSE,
						 pdTRUE,
						 portMAX_DELAY);
	
	UniversalClass::Task task;

	Plotter plot(0, 29, 0, 9, 1, 3, 0, 0,
		1, 0, 0, 24,
		0, 28, 0, 27,
		0, 10);

	while(1){
		xQueueReceive(xQueue, &task, portMAX_DELAY);
		if(task._Task == UniversalClass::M1){
			plot.pen(task._SubTask);
			vTaskDelay(configTICK_RATE_HZ / 10);
		}else if(task._Task == UniversalClass::G1){
			plot.abs(task._XCordinate, task._YCordinate);
		}else if(task._Task == UniversalClass::G28){
			plot.home();
		}else if(task._Task == UniversalClass::RESET){
			plot.reset();
		}else if(task._Task == UniversalClass::DISABLE_LIMIT_SAFETY){
			plot.safety(false);
		}else if(task._Task == UniversalClass::ENABLE_LIMIT_SAFETY){
			plot.safety(true);
		}else if(task._Task == UniversalClass::M4){
			ITM_write("Done\n");
		}else if(task._Task == UniversalClass::IX){
			plot.dif(task._SubTask, 0);
		}else if(task._Task == UniversalClass::IY){
			plot.dif(0, task._SubTask);
		}
	}
}

void task_init(){
	xTaskCreate(task, "task", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 2UL, NULL);
	xTaskCreate(receive_task, "Rx", configMINIMAL_STACK_SIZE*4, NULL, (tskIDLE_PRIORITY + 2UL),	(TaskHandle_t *) NULL);
	xTaskCreate(cdc_task, "CDC", configMINIMAL_STACK_SIZE*4, NULL, (tskIDLE_PRIORITY + 2UL), (TaskHandle_t *) &xTaskCDCHandle);
	// needs join memory - 16_32k
}

int main(void){
	prvSetupHardware();
	ITM_init();
	
	task_init();

	vTaskStartScheduler();
	return 1;
}
