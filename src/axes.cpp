#include "axes.h"

const int port_lim_x1 = 0;
const int  pin_lim_x1 = 0;
const int port_lim_x2 = 0;
const int  pin_lim_x2 = 0;

const int port_lim_y1 = 0;
const int  pin_lim_y1 = 0;
const int port_lim_y2 = 0;
const int  pin_lim_y2 = 0;


const int port_step_x = 0;
const int  pin_step_x = 0;

const int port_step_y = 0;
const int  pin_step_y = 0;


const int port_dir_x = 0;
const int  pin_dir_x = 0;

const int port_dir_y = 0;
const int  pin_dir_y = 0;



DigitalIoPin *lim_x1;
DigitalIoPin *lim_x2;
DigitalIoPin *lim_y1;
DigitalIoPin *lim_y2;

DigitalIoPin *step_x;
DigitalIoPin *step_y;

DigitalIoPin *dir_x;
DigitalIoPin *dir_y;


const bool Dir_1 = false;
const bool Dir_2 = true;

void axes_Init() {
	lim_x1 = new DigitalIoPin(port_lim_x1, pin_lim_x1, DigitalIoPin::pullup, true);
	lim_x2 = new DigitalIoPin(port_lim_x2, pin_lim_x2, DigitalIoPin::pullup, true);
	lim_y1 = new DigitalIoPin(port_lim_y1, pin_lim_y1, DigitalIoPin::pullup, true);
	lim_y2 = new DigitalIoPin(port_lim_y2, pin_lim_y2, DigitalIoPin::pullup, true);
	
	step_x = new DigitalIoPin(port_step_x, pin_step_x, DigitalIoPin::output, false);
	step_y = new DigitalIoPin(port_step_y, pin_step_y, DigitalIoPin::output, false);
	
	dir_x  = new DigitalIoPin(port_dir_x, pin_dir_x, DigitalIoPin::output, false);
	dir_y  = new DigitalIoPin(port_dir_y, pin_dir_y, DigitalIoPin::output, false);
}
