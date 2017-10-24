#ifndef SERVO_H_
#define SERVO_H_

#include "PWM.h"

class Servo
{
public:
	
	int operator+=(int);
	int operator+=(double);
	int operator=(int);
	int operator=(double);

private:

	int degree();

private:

	int min;
	int max;

	int level;

	int dif; // max - min

	int set(int);
	int inc(int);

	PWM pwm;

public:

	void start();
	void stop();

public:

	Servo(LPC_SCT_T * lpc_sct, int port, int pin);
	~Servo();
	
private:

	static uint32_t x1ms;
	static uint32_t x1d;
};

#endif