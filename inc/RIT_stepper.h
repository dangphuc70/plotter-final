#ifndef RIT_STEPPER_H_
#define RIT_STEPPER_H_

#include "FreeRTOS.h"
#include "semphr.h" // for sbRIT

extern int RIT_count;
extern SemaphoreHandle_t sbRIT;

void RIT_stepper_Init(void);

void RIT_start(void);

void RIT_set(DigitalIoPin * step_pin, int count, int us);
void RIT_set(int count, int us);



static inline void StopRITFromISR(portBASE_TYPE *ptrxHigherPriorityWoken) {
	Chip_RIT_Disable(LPC_RITIMER); // disable timer
	// Give semaphore and set context switch flag if a higher priority task was woken up
	xSemaphoreGiveFromISR(sbRIT, ptrxHigherPriorityWoken);
}


#endif /* RIT_STEPPER_H_ */
