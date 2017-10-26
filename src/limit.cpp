#include "Limit.h"
#include "rit.h"

DigitalIoPin *Limit::lim[4] = {NULL, NULL, NULL, NULL};

Limit::Limit(int port0, int pin0,
	  		 int port1, int pin1,
	  		 int port2, int pin2,
	  		 int port3, int pin3){
	// create pins
	lim[0] = new DigitalIoPin(port0, pin0, DigitalIoPin::pullup, true);
	lim[1] = new DigitalIoPin(port1, pin1, DigitalIoPin::pullup, true);
	lim[2] = new DigitalIoPin(port2, pin2, DigitalIoPin::pullup, true);
	lim[3] = new DigitalIoPin(port3, pin3, DigitalIoPin::pullup, true);

	// set up pin interrupts
	// Initialize pin-interrupts
	// step 1
	Chip_INMUX_PinIntSel(0, port0, pin0);
	Chip_INMUX_PinIntSel(1, port1, pin1);
	Chip_INMUX_PinIntSel(2, port2, pin2);
	Chip_INMUX_PinIntSel(3, port3, pin3);

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

	Limit::enable();
}

void Limit::disable(){
	NVIC_DisableIRQ(PIN_INT0_IRQn);
	NVIC_DisableIRQ(PIN_INT1_IRQn);
	NVIC_DisableIRQ(PIN_INT2_IRQn);
	NVIC_DisableIRQ(PIN_INT3_IRQn);
}

void Limit::enable(){
	NVIC_EnableIRQ(PIN_INT0_IRQn);
	NVIC_EnableIRQ(PIN_INT1_IRQn);
	NVIC_EnableIRQ(PIN_INT2_IRQn);
	NVIC_EnableIRQ(PIN_INT3_IRQn);
}

Limit::~Limit(){
	for(int i = 0; i < 4; ++i)
		delete lim[i];
}

bool Limit::operator()(int index){
	if(index >= 0 and index < 4){
		return lim[index]->read();
	}else{
		return false;
	}
}
DigitalIoPin * Limit::operator[](int index){
	if(index >= 0 and index < 4){
		return lim[index];
	}else{
		return NULL;
	}
}

void Limit::ISR(){
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	
	rit::StopFromISR(&xHigherPriorityWoken);
	rit::poweroff();
	
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

extern "C"
{
// Limit x1 IRQ Handler
void PIN_INT0_IRQHandler(void) {

	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH0);
		rit::StopFromISR(&xHigherPriorityWoken);

		portEND_SWITCHING_ISR(xHigherPriorityWoken);
}
// Limit x2 IRQ Handler
void PIN_INT1_IRQHandler(void) {

	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH1);
		rit::StopFromISR(&xHigherPriorityWoken);

		portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

// Limit y1 IRQ Handler
void PIN_INT2_IRQHandler(void) {

	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH2);
		rit::StopFromISR(&xHigherPriorityWoken);

		portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

// Limit y2 IRQ Handler
void PIN_INT3_IRQHandler(void) {

	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	Chip_PININT_ClearIntStatus(LPC_GPIO_PIN_INT, PININTCH3);
		rit::StopFromISR(&xHigherPriorityWoken);

		portEND_SWITCHING_ISR(xHigherPriorityWoken);
}
}
