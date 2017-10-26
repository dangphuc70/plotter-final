#include "chip.h"
#include "rit.h"

SemaphoreHandle_t rit::stop_b = xSemaphoreCreateBinary();
int rit::count = 0;
DigitalIoPin * rit::pin = NULL;
SemaphoreHandle_t rit::mutex = xSemaphoreCreateMutex();

void rit::ISR(void){
	// This used to check if a context switch is required
	portBASE_TYPE xHigherPriorityWoken = pdFALSE;
	// Tell timer that we have processed the interrupt.
	// Timer then removes the IRQ until next match occurs
	Chip_RIT_ClearIntStatus(LPC_RITIMER); // clear IRQ flag
	if(count > 0) {
		count--;
		// do something useful here (toggle)
		pin->write(!pin->read());
	}else{
		StopFromISR(&xHigherPriorityWoken);
	}
	// End the ISR and (possibly) do a context switch
	portEND_SWITCHING_ISR(xHigherPriorityWoken);
}

extern "C"{
void RIT_IRQHandler(void){
	rit::ISR();
}
}

portBASE_TYPE rit::take(TickType_t timeout){
	return xSemaphoreTake(mutex, timeout);
}
void rit::give(){
	xSemaphoreGive(mutex);
}

void rit::StopFromISR(portBASE_TYPE *ptr){
	Disable();
	xSemaphoreGiveFromISR(stop_b, ptr);
}
void rit::SetPin(DigitalIoPin * pin){
	rit::pin = pin;
}
void rit::SetCount(int count){
	rit::count = count;
}
void rit::SetPulsePerSecond(int pps){
	uint64_t cmp_value;
	// Determine approximate compare value based on clock rate and passed pulse per second
	cmp_value = (uint64_t) Chip_Clock_GetSystemClockRate() / (uint64_t) pps;
	cmp_value /= 2;
	
	// disable timer during configuration
	Chip_RIT_Disable(LPC_RITIMER);

	// enable automatic clear on when compare value==timer value
	// this makes interrupts trigger periodically
	Chip_RIT_EnableCompClear(LPC_RITIMER);
	// reset the counter
	Chip_RIT_SetCounter(LPC_RITIMER, 0);
	Chip_RIT_SetCompareValue(LPC_RITIMER, cmp_value);
}
void rit::Enable(){
	Chip_RIT_Enable(LPC_RITIMER);
	NVIC_EnableIRQ(RITIMER_IRQn);
}
void rit::Disable(){
	NVIC_DisableIRQ(RITIMER_IRQn);
	Chip_RIT_Disable(LPC_RITIMER);
}
void rit::Run(){
	Enable();

	// wait for stop
	if(WaitForStop(portMAX_DELAY)){
		Disable();
	}
}
void rit::SetRun(int pps, int count){
	SetPulsePerSecond(pps);
	SetCount(count);
	Run();
}
void rit::SetRun(int count){
	rit::Disable();
	Chip_RIT_SetCounter(LPC_RITIMER, 0);
	SetCount(count);
	Run();
}
void rit::SetRun(DigitalIoPin * pin, int pps, int count){
	SetPulsePerSecond(pps);
	SetCount(count);
	SetPin(pin);
	Run();
}
void rit::SetRun(DigitalIoPin * pin, int count){
	rit::Disable();
	Chip_RIT_SetCounter(LPC_RITIMER, 0);
	SetCount(count);
	SetPin(pin);
	Run();
}
portBASE_TYPE rit::WaitForStop(TickType_t timeout){
	return xSemaphoreTake(stop_b, timeout);
}
void rit::WritePin(bool value){
	pin->write(value);
}
void rit::init(){
	// initialize RIT (= enable clocking etc.)
	Chip_RIT_Init(LPC_RITIMER);
	// set the priority level of the interrupt
	// The level must be equal or lower than the maximum priority specified in FreeRTOS config
	// Note that in a Cortex-M3 a higher number indicates lower interrupt priority
	NVIC_SetPriority( RITIMER_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY + 1 );
}
rit::rit(DigitalIoPin * pin, int pps, int count){
	rit::init();
	rit::SetPulsePerSecond(pps);
	rit::SetPin(pin);
	rit::SetCount(count);
}

void rit::poweroff(){
	Chip_RIT_DeInit(LPC_RITIMER);
}

void rit::poweron(){
	Chip_RIT_Init(LPC_RITIMER);
}