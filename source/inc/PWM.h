#ifndef PWD_H_
#define PWD_H_

#include "chip.h"

class PWM
{
public:

	uint32_t operator=(uint32_t);
	uint32_t operator=(double);
	uint32_t period(uint32_t);
	uint32_t duty(uint32_t);
	uint32_t duty(double fraction);

	void halt();
	void unhalt();

public:

	uint32_t period();
	uint32_t duty();

private:

	LPC_SCT_T * ptr;
	CHIP_SWM_PIN_MOVABLE movable;

	int port;
	int pin;

	CHIP_SWM_PIN_MOVABLE select_movable();

public:

	void assign(int po, int pi);

public:

	PWM(LPC_SCT_T * p,
		int port_,
		int pin_,
		uint32_t period_t = Chip_Clock_GetSystemClockRate() / 50,
		uint32_t duty_t = Chip_Clock_GetSystemClockRate() / 500);
	~PWM();
	
};

#endif