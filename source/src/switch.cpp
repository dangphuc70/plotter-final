#include "switch.h"

const int port_switch_1 = 0;
const int  pin_switch_1 = 0;

const int port_switch_2 = 0;
const int  pin_switch_2 = 0;

const int port_switch_3 = 0;
const int  pin_switch_3 = 0;

DigitalIoPin *switch_1;
DigitalIoPin *switch_2;
DigitalIoPin *switch_3;

void Switch_Init()
{
	switch_1 = new DigitalIoPin(port_switch_1, pin_switch_1, DigitalIoPin::pullup, true);
	switch_2 = new DigitalIoPin(port_switch_2, pin_switch_2, DigitalIoPin::pullup, true);
	switch_3 = new DigitalIoPin(port_switch_3, pin_switch_3, DigitalIoPin::pullup, true);
}