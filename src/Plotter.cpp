#include "Plotter.h"
#include "chip.h"
#include "rit.h"
#include "BresenhamD.h"
#include <cmath>
Plotter::Plotter(int portlim0, int pinlim0,
				int portlim1, int pinlim1,
				int portlim2, int pinlim2,
				int portlim3, int pinlim3,
				int portdirx, int pindirx,
				int portstepx, int pinstepx,
				int portdiry, int pindiry,
				int portstepy, int pinstepy,
				int portpen, int pinpen)
	: lim(portlim0, pinlim0,
		  portlim1, pinlim1,
		  portlim2, pinlim2,
		  portlim3, pinlim3),
	  servo(LPC_SCTLARGE0, portpen, pinpen){

	step_x = new DigitalIoPin(portstepx, pinstepx, DigitalIoPin::output);
	step_y = new DigitalIoPin(portstepy, pinstepy, DigitalIoPin::output);
	dir_x  = new DigitalIoPin(portdirx, pindirx, DigitalIoPin::output);
	dir_y  = new DigitalIoPin(portdiry, pindiry, DigitalIoPin::output);

	x.SetDir(dir_x);
	y.SetDir(dir_y);
	x.SetStep(step_x);
	y.SetStep(step_y);

	x.SetLim0(lim[0]);
	x.SetLim1(lim[1]);
	y.SetLim0(lim[2]);
	y.SetLim1(lim[3]);

	x.mmStep(10000, 100.0);
	y.mmStep(10000, 100.0);

	servo = 20;

	rit RIT_init(NULL, 1000);
}

Plotter::~Plotter(){
	delete step_x;
	delete step_y;
	delete dir_x;
	delete dir_y;
}

void Plotter::operator()(int dx, int dy){
	int f = abs(dx);
	int e = abs(dy);

	Axis * B;
	Axis * H;

	if(f >= e){
		B = &x;
		H = &y;
	}else{
		B = &y;
		H = &x;
		int temp = dx;
		dx = dy;
		dy = temp;
	}
	BresenhamD line(B, H, dx, dy);
	f = abs(dx);
	if(f < 500){
		rit::SetPulsePerSecond(4000);
		line();
	}else{
		int d = 500;
		int a = d / 3;
		int t = f - d;
		int a1 = a / 3;
		int a2 = 2 * a1;
		int a3 = a - (a1 + a2);
		rit::SetPulsePerSecond(2000);
		line(a1);
		rit::SetPulsePerSecond(4000);
		line(a2);
		rit::SetPulsePerSecond(7000);
		line(a3);
		rit::SetPulsePerSecond(8000);
		line(t);
	}

}

void Plotter:: operator()(double xx, double yy){
	int dx;
	int dy;

	dx = int(xx / x.stepToMm(1));
	dy = int(yy / y.stepToMm(1));

	dx = dx - x();
	dy = dy - y();

	operator()(dx, dy);
}


void Plotter:: pen(int degree){
	servo = degree;
}

void Plotter::home(){
	x += -x();
	y += -y();
}

void Plotter::reset(){
	// information reset, assume pins and servo are functional and properly set
	x = 0;
	y = 0;
	servo = 20;

	// hardware reset
	rit::poweroff();
	rit::poweron();

	rit RIT_init(NULL, 1000);

}