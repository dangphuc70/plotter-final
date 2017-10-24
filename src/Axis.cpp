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

int Axis::operator+=(int delta){
	if(delta == 0) return coordinate;

	int end_coordinate = coordinate + delta;
	if(end_coordinate >= 0 and end_coordinate <= max){
		coordinate += delta;
		if(delta > 0){
			dir->write(Direction::Dir_1);
		}else{
			dir->write(Direction::Dir_0);
			delta = -delta;
		} // at this point, delta is > 0
		// toggle step pin from off state
		step->write(false);
		rit::SetRun(step, delta + delta - 1);
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

void Axis::VerifyLimit(Limit& lim){
	// find lim0
	for(bool found = false; !found; decrement()){
		for(int i = 0; i < 4; ++i){
			if(lim(i)){
				lim0 = lim[i];
				found = true;
				break;
			}
		}
	}

	// find lim1
	for(bool found = false; !found; increment()){
		for(int i = 0; i < 4; ++i){
			if(lim(i)){
				lim1 = lim[i];
				found = true;
				break;
			}
		}
	}
	
}