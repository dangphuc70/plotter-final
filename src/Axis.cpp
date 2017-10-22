#include "Axis.h"

Axis::Axis(DigitalIoPin * dir_,
		   DigitalIoPin * step_,
		   DigitalIoPin * lim0_,
		   DigitalIoPin * lim1_)
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
