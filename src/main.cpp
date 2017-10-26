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

const char _Answer[4]="OK\n";
QueueHandle_t xQueue;
Fmutex *mtx;
TaskHandle_t xTaskCDCHandle;

//Our functions

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
	
	Axis * x = new Axis(limits[0], limits[1], dir_x, step_x);
	Axis * y = new Axis(limits[2], limits[3], dir_y, step_y);

	Servo servo(LPC_SCTLARGE0, 0, 10);

	servo = 30;

	servo = 50;

	BresenhamD aline(x, y, 5000, 4000);
	aline();

	BresenhamD back(x, y, -5000, -4000);
	back(2000);
	vTaskDelay(1000);
	back(3000);


	vTaskDelay(portMAX_DELAY);
}

static void plotter_test(void *pvParameters){
	

	Plotter plot(0, 29, 0, 9, 1, 3, 0, 0,
		1, 0, 0, 24,
		0, 28, 0, 27,
		0, 10);

	plot.pen(25);
	plot(100.0,150.0);

	plot.pen(48);
	plot(150.0, 100.0);

	vTaskDelay(portMAX_DELAY);
}

static void receive_task(void *pvParameters)
{
	vTaskDelay(configTICK_RATE_HZ*2);
	vTaskPrioritySet(xTaskCDCHandle,(tskIDLE_PRIORITY + 1UL));
	uint32_t len;
	char c[80];
	int delay = configTICK_RATE_HZ/ 100;

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
	vTaskDelay(configTICK_RATE_HZ*2);
	vTaskDelay(configTICK_RATE_HZ*2);
	vTaskDelay(configTICK_RATE_HZ*2);
	UniversalClass::Task task;

	Plotter plot(0, 29, 0, 9, 1, 3, 0, 0,
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
		}
	}
}

void task_init(){
	xTaskCreate(task, "task", configMINIMAL_STACK_SIZE * 3, NULL, tskIDLE_PRIORITY + 2UL, NULL);
	xTaskCreate(receive_task, "Rx",
					configMINIMAL_STACK_SIZE*3, NULL, (tskIDLE_PRIORITY + 2UL),
					(TaskHandle_t *) NULL);

		xTaskCreate(cdc_task, "CDC",
						configMINIMAL_STACK_SIZE*2, NULL, (tskIDLE_PRIORITY + 2UL),
						(TaskHandle_t *) &xTaskCDCHandle);
}

int main(void){
	prvSetupHardware();
	ITM_init();
	
	task_init();

	vTaskStartScheduler();
	return 1;
}
