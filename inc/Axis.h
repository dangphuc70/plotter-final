#ifndef AXIS_H_
#define AXIS_H_

#include "DigitalIoPin.h"
#include "Direction.h"

class Axis
{

	private:

		DigitalIoPin * dir;
		DigitalIoPin * step;
		DigitalIoPin * lim0;
		DigitalIoPin * lim1;

	public:

		Direction DirectionObject();

	public:

		Axis(DigitalIoPin * lim0_,
			 DigitalIoPin * lim1_,
			 DigitalIoPin * dir_,
			 DigitalIoPin * step_);
		~Axis();
	
};

#endif