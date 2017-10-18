#include "chip.h"

#include "ITM_write.h"

#include "axes.h" // for port and pin numbers of lim_x1, lim_x2, lim_y1, lim_y2
				  // for Direction consts
				  // for limit pins initialization
#include "pin_interrupt_limits.h"
#include "RIT_stepper.h" // for Give_sbRITFromISR ()

#include "FreeRTOS.h"
#include "queue.h" // for queue
#include "task.h" // for task

enum SwitchNumber {
	eX1,
	eX2,
	eY1,
	eY2
};

const SwitchNumber X1 = eX1;
const SwitchNumber X2 = eX2;
const SwitchNumber Y1 = eY1;
const SwitchNumber Y2 = eY2;

static QueueHandle_t SwitchNumber_q;


// block 1 : Initialize pin interrupt
// for limit switch (4 limits)
void PIN_INT_Init(void)
{
	// Initialize pin-interrupts
	// step 1
	Chip_INMUX_PinIntSel(0, port_lim_x1, pin_lim_x1);
	Chip_INMUX_PinIntSel(1, port_lim_x2, pin_lim_x2);
	Chip_INMUX_PinIntSel(2, port_lim_y1, pin_lim_y1);
	Chip_INMUX_PinIntSel(3, port_lim_y2, pin_lim_y2);

	// step 2 : initialize LPC_GPIO_PIN_INT, enable rising edge interrupt
	Chip_PININT_Init(LPC_GPIO_PIN_INT);
	// enable rising edge interrupt
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH0);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH1);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH2);
	Chip_PININT_EnableIntHigh(LPC_GPIO_PIN_INT, PININTCH3);

	// step 3 : set priorities for pin int IRQ handlers
	NVIC_SetPriority(PIN_INT0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
	NVIC_SetPriority(PIN_INT3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );

	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT3_IRQn);

	SwitchNumber_q = xQueueCreate(5, sizeof(int));

}

void pin_int_limit_test(void *pvParameters)
{
	axes_Init();
	PIN_INT_Init();
	ITM_init();

	SwitchNumber switch_number;
	while(1)
	{
		xQueueReceive(SwitchNumber_q, &switch_number, portMAX_DELAY);
		switch( switch_number )
		{
			case eX1:
				ITM_write("X1\n");
				break;
			case eX2:
				ITM_write("X2\n");
				break;
			case eY1:
				ITM_write("Y1\n");
				break;
			case eY2:
				ITM_write("Y2\n");
				break;
		}
	}
}

void pin_int_limit_itm_test_Init(void)
{
	xTaskCreate(pin_int_limit_test, "pin_int_limit_test", configMINIMAL_STACK_SIZE * 4, NULL, tskIDLE_PRIORITY + 1UL, NULL);
}

// block 2 : IRQ Handler for each limits in limit test using ITM indication
extern "C"
{
	// limit x1 IRQ Handler
	void PIN_INT0_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		// step 3 : send switch number to queue
		xQueueSendToBackFromISR(SwitchNumber_q, (void*) &X1 , &xHigherPriorityWoken);

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
	
	// limit x2 IRQ Handler
	void PIN_INT1_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		// step 3 : send switch number to queue
		xQueueSendToBackFromISR(SwitchNumber_q, (void*) &X2 , &xHigherPriorityWoken);

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
	
	// limit y1 IRQ Handler
	void PIN_INT2_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		// step 3 : send switch number to queue
		xQueueSendToBackFromISR(SwitchNumber_q, (void*) &Y1 , &xHigherPriorityWoken);

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
	
	// limit y2 IRQ Handler
	void PIN_INT3_IRQHandler(void) {

		// step 2 : prepare for END_SWITCHING_ISR
		portBASE_TYPE xHigherPriorityWoken = pdFALSE;
		
		// step 3 : send switch number to queue
		xQueueSendToBackFromISR(SwitchNumber_q, (void*) &Y2 , &xHigherPriorityWoken);

		// step :
		Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
		
		// step 4 : end the ISR and (possibly) do a context switch
		portYIELD_FROM_ISR(xHigherPriorityWoken);
	}
}
