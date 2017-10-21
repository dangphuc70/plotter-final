#include "Axis.h"

Axis::Axis(const AxisDesc& axis)
{

}

Axis::~Axis(){

}

Direction Axis::DirectionObject(){
	return Direction(lim0, lim1, dir);
}

DigitalIoPin * Axis::digi(const PinDesc& pin){
	return new DigitalIoPin(pin.port, pin.pin, pin.mode, pin.invert);
}