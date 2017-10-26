#include "Axis.h"
#include "rit.h"

Axis::Axis(DigitalIoPin * lim0_,
		   DigitalIoPin * lim1_,
		   DigitalIoPin * dir_,
		   DigitalIoPin * step_,
		   int coordinate_)
	: dir(dir_),
	  step(step_),
	  lim0(lim0_),
	  lim1(lim1_),
	  coordinate(coordinate_){

}

Axis::~Axis(){

}

Direction Axis::DirectionObject(){
	return Direction(lim0, lim1, dir);
}

int Axis::operator()(){
	return coordinate;
}

int Axis::operator=(int coordinate_){
	if(coordinate_ >= 0)
		coordinate = coordinate_;
	return coordinate;
}

int Axis::operator+=(int delta){
	if(delta == 0) return coordinate;

	increment(delta);

	return coordinate;
}
int Axis::operator-=(int delta){
	return operator+=(-delta);
}



void Axis::increment(){
	dir->write(Direction::Dir_1);
	step->write(false);
	rit::SetRun(step, 1);
	coordinate++;
}
void Axis::decrement(){
	dir->write(Direction::Dir_0);
	step->write(false);
	rit::SetRun(step, 1);
	coordinate--;
}

void Axis::increment(int delta){
	if(delta == 0)
		return;

	int nstep;
	if(delta > 0){
		dir->write(Direction::Dir_1);
		nstep = delta;
	}else{
		dir->write(Direction::Dir_0);
		nstep = -delta;
	}

	step->write(false);
	rit::SetRun(step, nstep + nstep - 1);
	coordinate += delta;
}

void Axis::decrement(int delta){
	increment(-delta);
}



bool Axis::FindLimit0(Limit& lim){

	return FindLimit0(lim, 200);
}

bool Axis::FindLimit1(Limit& lim){

	return FindLimit1(lim, 200);
}


// redo these two
// step 1 : disable limits
// step 2 : drive 1 step in one direction, check all limits, loop until one hits
bool Axis::FindLimit0(Limit& lim, int pps){


	return lim0 != NULL;
}

bool Axis::FindLimit1(Limit& lim, int pps){



	return lim1 != NULL;
}

void Axis::SetLim0(DigitalIoPin * l0){
	lim0 = l0;
}

void Axis::SetLim1(DigitalIoPin * l1){
	lim1 = l1;
}

void Axis::SetDir(DigitalIoPin * d){
	dir = d;
}

void Axis::SetStep(DigitalIoPin * s){
	step = s;
}


int Axis::mmToStep(double mm){
	return mm / mmStepVar;
}

double Axis::stepToMm(int n){
	return n * mmStepVar;
}

void Axis::mmStep(int nstep, double mm){
	mmStepVar = mm / nstep;
}