#ifndef LIMIT_H_
#define LIMIT_H_

#include "DigitalIoPin.h"

class Limit
{
private:

	DigitalIoPin *lim[4];

public:

	static void ISR(void);
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
