#ifndef SWITCH_H_
#define SWITCH_H_
#include "DigitalIoPin.h"

extern const int port_switch_1;
extern const int  pin_switch_1;

extern const int port_switch_2;
extern const int  pin_switch_2;

extern const int port_switch_3;
extern const int  pin_switch_3;

extern DigitalIoPin *switch_1;
extern DigitalIoPin *switch_2;
extern DigitalIoPin *switch_3;

void Switch_Init();
#endif /* SWITCH_H_ */
