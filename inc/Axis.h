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

	private:

		double mmStepVar;

	public:

		int mmToStep(double mm);
		double stepToMm(int n);
		void mmStep(int nstep, double mm);

	public:

		bool FindLimit0(Limit&);
		bool FindLimit1(Limit&);
		bool FindLimit0(Limit&, int pps);
		bool FindLimit1(Limit&, int pps);
		void SetLim0(DigitalIoPin * l0);
		void SetLim1(DigitalIoPin * l1);
		void SetDir(DigitalIoPin * d);
		void SetStep(DigitalIoPin * s);

	public:
		
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

	public:

		Axis(DigitalIoPin * lim0_ = NULL,
			 DigitalIoPin * lim1_ = NULL,
			 DigitalIoPin * dir_ = NULL,
			 DigitalIoPin * step_ = NULL,
			 int coordinate_ = 0);
		~Axis();
	
};

#endif