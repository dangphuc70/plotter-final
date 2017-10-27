#include "Plotter.h"
#include "chip.h"
#include "rit.h"
#include "BresenhamD.h"
namespace cmath{
	inline int abs(int x){
		return (x < 0) ? (-x) : (x);
	}
};
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

	x.mmStep(3000, 35.0);
	y.mmStep(3000, 35.0);

	servo = 20;

	rit RIT_init(NULL, 1000);
}

Plotter::~Plotter(){
	delete step_x;
	delete step_y;
	delete dir_x;
	delete dir_y;
}

void Plotter::dif(int dx, int dy){
	int f = cmath::abs(dx);
	int e = cmath::abs(dy);

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
	f = cmath::abs(dx);
	if(f < 500){
		rit::SetPulsePerSecond(4000);
		line();
	}else{
		int d = 500;
		int a1 = d / 3;
		int a2 = 2 * a1;
		int a3 = d - (a1 + a2);

		int t = f - d;

		rit::SetPulsePerSecond(2000);
		line(a1);
		rit::SetPulsePerSecond(4000);
		line(a2);
		rit::SetPulsePerSecond(6000);
		line(a3);
		rit::SetPulsePerSecond(8000);
		line(t);
	}

}

void Plotter:: dif(double ddx, double ddy){
	int dx;
	int dy;

	dx = int(ddx / x.stepToMm(1));
	dy = int(ddy / y.stepToMm(1));

	dif(dx, dy);
}
void Plotter:: abs(double xx, double yy){
	int ax;
	int ay;

	ax = int(xx / x.stepToMm(1));
	ay = int(yy / y.stepToMm(1));

	Plotter::abs(ax, ay);
}
void Plotter:: abs(int xx, int yy){
	int dx;
	int dy;

	dx = xx - x();
	dy = yy - y();

	dif(dx, dy);
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

void Plotter::safety(bool on){
	if(on){
		Limit::enable();
	}else{
		Limit::disable();
	}
}
