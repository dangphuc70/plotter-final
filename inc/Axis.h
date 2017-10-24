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

		bool FindLimit0(Limit&);
		bool FindLimit1(Limit&);
		Direction DirectionObject();
		int operator()();
		int operator=(int coordinate_);

	public:

		int operator+=(int delta);
		int operator-=(int delta);

	public: // all unchecked against max (value)

		void increment();
		void decrement();
		void increment(int delta);
		void decrement(int delta);
		void free_fall(bool Dir_b);

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