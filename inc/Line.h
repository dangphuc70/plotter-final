#ifndef LINE_H_
#define LINE_H_

#include "Axis.h"
#include <cstdlib>
#include <cstdio>
#include "ITM_write.h"


struct BresenhamD {
	int D;
	int dx;
	int dy;
	int x;
	int y;
	int inc_x;
	int inc_y;
	int updateless;
	int updatemore;

	Axis * b;
	Axis * o;

	void init(){
		y = 0;
		x = 0;
		updateless = dy + dy;
		updatemore = updateless - dx - dx;
		D = updatemore + dx;
	}

	void init(int ddx, int ddy){
		dx = abs(ddx);
		dy = abs(ddy);
		if(dx < dy){
			int t = dx;
			dx = dy;
			dy = t;
		}
	}

	void init(Axis * bb, int ix, Axis * oo, int iy){
		b = bb;
		o = oo;
		inc_x = ix;
		inc_y = iy;
	}

	void init(Axis * bb, Axis * oo, int ddx, int ddy){
		init(bb, 1, oo, 1);
		init(ddx, ddy);
		init();
	}

	bool update(){
		if(x >= dx){
			return false;
		}else{
			x += 1;
			(*b) += inc_x;
			if(D < 0){
				D = D + updateless;
			}else{
				D = D + updatemore;
				(*o) += inc_y;
			}
			return true;
		}
	}

	void print(){
		static char space[40];
		snprintf(space, 40,
				 "%10d %10d %10d\n", D, y, x);
		ITM_write(space);
	}
};

#endif