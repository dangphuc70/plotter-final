#include "Axis.h"
#include "rit.h"

Axis::Axis(DigitalIoPin * lim0_,
		   DigitalIoPin * lim1_,
		   DigitalIoPin * dir_,
		   DigitalIoPin * step_,
		   int max_,
		   int coordinate_)
	: dir(dir_),
	  step(step_),
	  lim0(lim0_),
	  lim1(lim1_),
	  coordinate(coordinate_),
	  max((max_ > 0) ? max_ : (-max_)){

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

bool Axis::delta_check(int delta){
	int end_coordinate = coordinate + delta;
	return (end_coordinate >= 0 and end_coordinate <= max);
}

int Axis::operator+=(int delta){
	if(delta == 0) return coordinate;

	if(delta_check(delta)){
		increment(delta);
	}
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
	if(delta == 0) return;

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

void Axis::free_fall(bool Dir_b, int pps){
	dir->write(Dir_b);
	step->write(false);
	rit::SetRun(step, pps, 100000);
}

void Axis::free_fall(bool Dir_b){
	free_fall(Dir_b, 50);
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

	Limit::latest_lim();
	free_fall(Direction::Dir_0, pps);
	lim0 = Limit::latest_lim();

	return lim0 != NULL;
}

bool Axis::FindLimit1(Limit& lim, int pps){

	Limit::latest_lim();
	free_fall(Direction::Dir_1, pps);
	lim1 = Limit::latest_lim();

	return lim1 != NULL;
}
