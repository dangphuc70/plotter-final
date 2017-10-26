#ifndef LINE_H_
#define LINE_H_

#include "Axis.h"
#include <cstdlib>
#include <cstdio>
#include "ITM_write.h"


class BresenhamD {

// geometry
	int ib;
	int io;
	Axis * ab;
	Axis * ao;


// algorithm
	int D;
	int updateless;
	int updatemore;
	int i;
	int time;

public:
	BresenhamD(Axis * base, Axis * other,
			   int distance_base, int distance_other){
		ab = base;
		ao = other;

		if(distance_base < 0){
			distance_base = -distance_base;
			ib = -1;
		}else{
			ib = 1;
		}

		if(distance_other < 0){
			distance_other = -distance_other;
			io = -1;
		}else{
			io = 1;
		}

		updateless = 2 * distance_other;
		updatemore = updateless - 2 * distance_base;

		D = updatemore + distance_base;

		i = 0;
		time = distance_base;
	}

public:

	void operator()(){
		while(update());
	}

	void operator()(int times){
		i = 0;
		time = times;
		while(update());
	}

public:

	bool update(){
		if(i >= time){ // algorithm
			return false;
		}else{
			i += 1; // algorithm
			(*ab) += ib; // geometry
			if(D < 0){
				D = D + updateless;
			}else{
				(*ao) += io; // geometry
				D = D + updatemore;
			}
			return true;
		}
	}

};

#endif