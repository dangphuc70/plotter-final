#ifndef RIT_H_
#define RIT_H_

#include "FreeRTOS.h"
#include "semphr.h"
#include "DigitalIoPin.h"

class rit
{
	private:

		static SemaphoreHandle_t stop_b;
		static int count;
		static DigitalIoPin * pin;

	public:

		static void StopFromISR(portBASE_TYPE *ptr);
		static void SetPin(DigitalIoPin * pin);
		static void SetPin(bool value);
		static void SetCount(int count);
		static void SetPulsePerSecond(int pps); // will stop RIT from counting
		static void Enable();
		static void Disable();
		static void Run();
		static void SetRun(int count, int pps);
		static void SetRun(DigitalIoPin * pin, int count, int pps);
		static portBASE_TYPE WaitForStop(TickType_t);
		static void init();

	public:
		static void ISR(void);
	
};

#endif