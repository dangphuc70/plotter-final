#ifndef AXES_H
#define AXES_H
#include "DigitalIoPin.h"

extern const int port_lim_x1;
extern const int  pin_lim_x1;
extern const int port_lim_x2;
extern const int  pin_lim_x2;

extern const int port_lim_y1;
extern const int  pin_lim_y1;
extern const int port_lim_y2;
extern const int  pin_lim_y2;


extern const int port_step_x;
extern const int  pin_step_x;

extern const int port_step_y;
extern const int  pin_step_y;

extern const int port_dir_x;
extern const int  pin_dir_x;

extern const int port_dir_y;
extern const int  pin_dir_y;



extern DigitalIoPin *lim_x1;
extern DigitalIoPin *lim_x2;
extern DigitalIoPin *lim_y1;
extern DigitalIoPin *lim_y2;

extern DigitalIoPin *step_x;
extern DigitalIoPin *step_y;

extern DigitalIoPin *dir_x;
extern DigitalIoPin *dir_y;

extern const bool Dir_1;
extern const bool Dir_2;

void axes_Init();

#endif