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
		static SemaphoreHandle_t mutex;

	public:

		static portBASE_TYPE take(TickType_t timeout);
		static void give();

	public:

		static void poweroff();
		static void poweroffFromISR(portBASE_TYPE *ptr);
		static void poweron();
		static void StopFromISR(portBASE_TYPE *ptr);
		static portBASE_TYPE WaitForStop(TickType_t);
		
	public:

		static void SetPin(DigitalIoPin * pin);
		static void SetCount(int count);
		static void SetPulsePerSecond(int pps); // will stop RIT from counting
		
	public:

		static void Enable();
		static void Disable();
		static void Run();
		
	public:

		static void SetRun(int pps, int count);
		static void SetRun(int count);
		static void SetRun(DigitalIoPin * pin, int pps, int count);
		static void SetRun(DigitalIoPin * pin, int count);
		
	public:

		static void init();

	public:
		
		static void ISR(void);

	public:
		
		static void WritePin(bool value);

	public:

		rit(DigitalIoPin * pin, int pps, int count = 0);
	
};

#endif