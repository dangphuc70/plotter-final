#ifndef AXIS_H_
#define AXIS_H_

#include "DigitalIoPin.h"
#include "Direction.h"
#include "limit.h"

class Axis
{

	private:

		DigitalIoPin * dir;
		DigitalIoPin * step;
		DigitalIoPin * lim0;
		DigitalIoPin * lim1;
		int coordinate;
		int max;

	public:

		void VerifyLimit(Limit&);
		Direction DirectionObject();
		int operator()();

	public:

		int operator+=(int delta);
		int operator-=(int delta);

	public:

		void increment();
		void decrement();

	public:

		Axis(DigitalIoPin * lim0_,
			 DigitalIoPin * lim1_,
			 DigitalIoPin * dir_,
			 DigitalIoPin * step_,
			 int max_,
			 int coordinate_ = 0);
		~Axis();
	
};

#endif