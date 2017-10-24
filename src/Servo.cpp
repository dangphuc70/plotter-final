#include "Servo.h"

uint32_t Servo::x1ms = Chip_Clock_GetSystemClockRate() / 1000;
uint32_t Servo::x1d;

int Servo::set(int level_){
	if(level_ > max){
		level = max;
	}else
	if(level_ < min){
		level = min;
	}else{
		level = level_;
	}
	pwm.duty(uint32_t(level));
	return level;
}

int Servo::inc(int delta){
	int level_ = level + delta;
	return set(level_);
}

Servo::Servo(LPC_SCT_T * lpc_sct,
			 int port,
			 int pin)
	: pwm(lpc_sct,
		  port,
		  pin){
	uint32_t period = 20 * x1ms;

	min = x1ms;
	max = 2 * x1ms;
	dif = max - min;
	x1d = dif / 180;

	uint32_t mid = min + (dif / 2);

	pwm.period(period);
	set(int(mid));

}

Servo::~Servo(){

}

void Servo::start(){
	pwm.unhalt();
}

void Servo::stop(){
	pwm.halt();
}

int Servo::operator+=(int deg){
	return operator=(int(level + (min + deg * x1d)));
}

int Servo::operator+=(double fraction){
	return operator=((double(level - min) / double(dif)) + fraction);
}

int Servo::operator=(int deg){
	set(int(min + deg * x1d));
	return degree();
}

int Servo::operator=(double fraction){
	set(int(min + fraction * dif));
	return degree();
}

int Servo::degree(){
	return (level - min) / x1d;
}