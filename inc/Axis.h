#ifndef AXIS_H_
#define AXIS_H_

#include "DigitalIoPin.h"
#include "Direction.h"
#include "desc.h"

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

		Axis(const AxisDesc& axis);
		~Axis();
	
	private:

		DigitalIoPin * digi(const PinDesc& pin);
};
#endif