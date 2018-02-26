#include "PWM.h"

uint32_t PWM::operator=(uint32_t cmp){
	return duty(cmp);
}

uint32_t PWM::operator=(double fraction){
	return duty(fraction);
}

uint32_t PWM::duty(double fraction){
	uint32_t cmp = uint32_t(fraction * period());
	return duty(cmp);
}

void PWM::halt(){
	ptr->CTRL_U |= (1 << 2);
}

void PWM::unhalt(){
	ptr->CTRL_U &= ~(1 << 2);
}

uint32_t PWM::period(uint32_t tick){
	ptr->MATCHREL[0].U = tick;
	return ptr->MATCHREL[0].U;
}

uint32_t PWM::duty(uint32_t tick){
	ptr->MATCHREL[1].U = tick;
	return ptr->MATCHREL[1].U;	
}

uint32_t PWM::period(){
	return ptr->MATCHREL[0].U;
}

uint32_t PWM::duty(){
	return ptr->MATCHREL[1].U;
}

void PWM::assign(int port_, int pin_){
	port = port_;
	pin = pin_;
	Chip_SWM_Init();
	Chip_SWM_MovablePortPinAssign(movable, port_, pin_);
	Chip_SWM_Deinit();
}

CHIP_SWM_PIN_MOVABLE PWM::select_movable(){
	if(ptr == LPC_SCT0)
		return SWM_SCT0_OUT0_O;
	else if(ptr == LPC_SCT1)
		return SWM_SCT1_OUT0_O;
	else if(ptr == LPC_SCT2)
		return SWM_SCT2_OUT0_O;
	else if(ptr == LPC_SCT3)
		return SWM_SCT3_OUT0_O;
	else
		return SWM_SCT0_OUT0_O;
}

PWM::PWM(LPC_SCT_T * p,
		 int port_,
		 int pin_,
		 uint32_t period_t,
		 uint32_t duty_t)
	: ptr(p),
	  port(port_),
	  pin(pin_){
	Chip_SCT_Init(ptr);
	ptr->CONFIG = (1 << 0);
	ptr->MATCHREL[0].U = period_t;
	ptr->MATCHREL[1].U = duty_t;
	ptr->LIMIT |= (1 << 0);
	ptr->EVENT[0].CTRL = (1 << 12);
	ptr->EVENT[1].CTRL = (1 << 12)
					   | (1 << 0);
	ptr->EVENT[0].STATE = 0xFFFFFFFF;
	ptr->EVENT[1].STATE = 0xFFFFFFFF; 
	ptr->OUT[0].SET = (1 << 0);
	ptr->OUT[0].CLR = (1 << 1);

	movable = select_movable();
	assign(port, pin);
}

PWM::~PWM(){
	Chip_SCT_DeInit(ptr);
}