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

const char _Answer[4]="OK\n";
QueueHandle_t xQueue;
Fmutex *mtx;
TaskHandle_t xTaskCDCHandle;

// initialize
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



void sendOK()
{
	USB_send((uint8_t *)_Answer, 3);
}

void CreateAndSendTask(char* line, int len)
{
	UniversalClass::Task t;
	t._Task = UniversalClass::ERROR;
	t._SubTask = UniversalClass::ERROR;
	t._XCordinate = 0;
	t._YCordinate = 0;

	std::string str(line,len);
	std::string commands[4] = {};
	int j = 0;

	for (int i = 0; i < len; i++)
	{
		if (str[i] != ' ')
		{
			commands[j] = commands[j] + str[i];
		}
		else
		{
			j++;
		}
	}
	j++;

	for (int i = 0; i < j; i++)
	{

		if (commands[i][0] == 'X')
		{
			commands[i].erase(0, 1);
			t._XCordinate = std::strtod(commands[i].c_str(),NULL);
		}
		else if (commands[i][0] == 'Y')
		{
			commands[i].erase(0, 1);
			t._YCordinate = std::strtod(commands[i].c_str(),NULL);
		}
		else if (commands[i][0] == 'A' && commands[i][1] == '0')
		{
			t._SubTask = UniversalClass::A0;
		}
		else if (commands[i][0] == 'G' && commands[i][1] == '1')
		{
			t._Task = UniversalClass::G1;
		}
		else if (commands[i][0] == 'M' && commands[i][1] == '1' && (commands[i].length() > 2) )
		{
			t._Task = UniversalClass::M10;
		}
		else if (commands[i][0] == 'M' && commands[i][1] == '1')
		{
			t._Task = UniversalClass::M1;
		}
		else if (commands[i][0] == 'M' && commands[i][1] == '4')
		{
			t._Task = UniversalClass::M4;
		}
		else if (commands[i][0] == 'G' && commands[i][1] == '2')
		{
			t._Task = UniversalClass::G28;
		}
		else if (strcmp(commands[i].c_str(), "RESET") == 0)
		{
			t._Task = UniversalClass::RESET;
		}
		else
		{
			if (std::isdigit(commands[i][0]))
			{
				t._SubTask = std::strtod(commands[i].c_str(),NULL);
			}
		}

	}
	xQueueSendToBack( xQueue, &t,  portMAX_DELAY);
	sendOK();
}





static void receive_task(void *pvParameters)
{
	xEventGroupWaitBits( cdc_sync,
						 (1 << 0),
						 pdFALSE,
						 pdTRUE,
						 portMAX_DELAY);
	vTaskPrioritySet(xTaskCDCHandle,(tskIDLE_PRIORITY + 1UL));
	uint32_t len;
	char c[80];

	xEventGroupSetBits( io_sync,
						(1 << 0) );

	while(true)
	{
		len = USB_receive((uint8_t *)c, 79);
		if (len > 0)
		{
			c[len] = 0;
			ITM_write(c);
			CreateAndSendTask(c,len);
		}
	}
}

static void task(void *pvParameters){

//	xEventGroupWaitBits( io_sync,
//						 (1 << 0),
//						 pdFALSE,
//						 pdTRUE,
//						 portMAX_DELAY);
	
	UniversalClass::Task task;

	Plotter plot(0, 17, 0, 9, 1, 3, 0, 0,
		1, 0, 0, 24,
		0, 28, 0, 27,
		0, 10);

	while(1){
		xQueueReceive(xQueue, &task, portMAX_DELAY);
		if(task._Task == UniversalClass::M1){
			plot.pen(task._SubTask);
		}else if(task._Task == UniversalClass::G1){
			plot(task._XCordinate, task._YCordinate);
		}else if(task._Task == UniversalClass::G28){
			plot.home();
		}else if(task._Task == UniversalClass::RESET){
			plot.reset();
		}
	}
}

void task_init(){
	xTaskCreate(task, "task", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 2UL, NULL);
	// xTaskCreate(receive_task, "Rx",
	// 				configMINIMAL_STACK_SIZE*4, NULL, (tskIDLE_PRIORITY + 2UL),
	// 				(TaskHandle_t *) NULL);

	// 	xTaskCreate(cdc_task, "CDC",
	// 					configMINIMAL_STACK_SIZE*4, NULL, (tskIDLE_PRIORITY + 2UL),
	// 					(TaskHandle_t *) &xTaskCDCHandle);
}

int main(void){
	prvSetupHardware();
	ITM_init();
	
	task_init();

	vTaskStartScheduler();
	return 1;
}
