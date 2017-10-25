#ifndef LIMIT_H_
#define LIMIT_H_

#include "FreeRTOS.h"
#include "queue.h"
#include "DigitalIoPin.h"

class Limit
{
private:

	static DigitalIoPin *lim[4];

	static QueueHandle_t lim_q;

public:

	static void ISR(int lim_number);
	static DigitalIoPin * latest_lim();
	static void disable();
	static void enable();

public:

	bool operator()(int index);
	DigitalIoPin * operator[](int index);

public:

	Limit(int port0, int pin0,
		  int port1, int pin1,
		  int port2, int pin2,
		  int port3, int pin3);
	~Limit();
	
};

#endif