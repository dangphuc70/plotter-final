#ifndef RIT_STEPPER_H_
#define RIT_STEPPER_H_
#include "FreeRTOS.h"
#include "semphr.h"

#include "DigitalIoPin.h"

extern SemaphoreHandle_t sbRIT;

void Stop_binary_RIT_Init(void);

void RIT_start_toggle(DigitalIoPin *, int count, int us);

#endif /* RIT_STEPPER_H_ */
