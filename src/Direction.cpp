#include "Direction.h"

const bool Direction::Dir_0 = false;
const bool Direction::Dir_1 =  true;

bool Direction::operator()(bool Dir_t){
	if(Dir_t){
		if(!lim1->read())
		{
			dir->write(Dir_1);
			return true;
		}else{
			return false;
		}
	}else{
		if(!lim0->read())
		{
			dir->write(Dir_0);
			return true;
		}else{
			return false;
		}
	}
}

bool Direction::operator()(){
	bool direction;

	direction = dir->read();
	
	if(direction)
		if(lim1->read()) return false;
	else
		if(lim0->read()) return false;
	return true;
}

int Direction::show_code(){
	int result = 0;
	if(lim0->read())
		result = 100;
	if(dir->read())
		result = result + 10;
	if(lim1->read())
		result = result + 1;
	return result;
}

Direction::Direction(DigitalIoPin * lim0_,
				  	 DigitalIoPin * lim1_,
				  	 DigitalIoPin * dir_)
	: lim0(lim0_),
	  lim1(lim1_),
	  dir(dir_){

}

Direction::~Direction(){

}
