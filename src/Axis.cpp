#include "Axis.h"

Axis::Axis(DigitalIoPin * lim0_,
		   DigitalIoPin * lim1_,
		   DigitalIoPin * dir_,
		   DigitalIoPin * step_)
	: dir(dir_),
	  step(step_),
	  lim0(lim0_),
	  lim1(lim1_){

}

Axis::~Axis(){

}

Direction Axis::DirectionObject(){
	return Direction(lim0, lim1, dir);
}
