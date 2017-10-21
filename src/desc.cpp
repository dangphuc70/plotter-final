#include "desc.h"

PinDesc::PinDesc(int port_,
				 int pin_,
				 DigitalIoPin::pinMode mode_,
				 bool invert_)
	: port(port_),
	  pin(pin_),
	  mode(mode_),
	  invert(invert_){

}

AxisDesc::AxisDesc(const PinDesc& dir_,
				   const PinDesc& step_,
				   const PinDesc& lim0_,
				   const PinDesc& lim1_)
	: dir(dir_),
	  step(step_),
	  lim0(lim0_),
	  lim1(lim1_){

}

PinDesc::~PinDesc(){

}

AxisDesc::~AxisDesc(){

}