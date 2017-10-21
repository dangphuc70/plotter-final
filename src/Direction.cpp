#include "Direction.h"

const bool Direction::Dir_0 = false;
const bool Direction::Dir_1 =  true;

bool operator()(bool Dir_t){
	if(Dir_t)
		if(!lim1->read())
		{
			dir->write(Dir_1);
			return true;
		}
	else
		if(!lim0->read())
		{
			dir->write(Dir_0);
			return true;
		}
	return false;
}

bool operator()(){
	bool direction;

	direction = dir->read();
	
	if(direction)
		if(lim1->read()) return false;
	else
		if(lim0->read()) return false;
	return true;
}