#ifndef DIRECTION_H_
#define DIRECTION_H_

#include "DigitalIoPin.h"

class Direction
{
	public:
		
		static const bool Dir_0;
		static const bool Dir_1;

	private:
		
		DigitalIoPin * lim0;
		DigitalIoPin * lim1;
		DigitalIoPin * dir;

	public:
		
		bool operator()(bool Dir_t);
		bool operator()();

	public:
		
		Direction();
		~Direction();
	
};
#endif