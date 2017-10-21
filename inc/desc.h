#ifndef DESC_H_
#define DESC_H_

#include "DigitalIoPin.h"

class PinDesc
{
	public:

		int port;
		int pin;
		DigitalIoPin::pinMode mode;
		bool invert;

	public:

		PinDesc(int port_, int pin_, DigitalIoPin::pinMode mode_, bool invert_);
		~PinDesc();
	
};

class AxisDesc
{
	public:

		PinDesc dir;
		PinDesc step;
		PinDesc lim0;
		PinDesc lim1;

	public:

		AxisDesc(const PinDesc& dir_
				 const PinDesc& step_,
				 const PinDesc& lim0_
				 const PinDesc& lim1_);
		~AxisDesc();
	
};
#endif