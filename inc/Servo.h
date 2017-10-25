#ifndef SERVO_H_
#define SERVO_H_

#include "PWM.h"

class Servo
{
public: // arguments are in degree for int and fraction in double
	
	int operator+=(int);
	int operator+=(double);
	int operator=(int);
	int operator=(double);

public:

	void set_duty(uint32_t);

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