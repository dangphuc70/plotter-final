#ifndef LINE_H_
#define LINE_H_

#include "Axis.h"
#include <cstdlib>
#include <cstdio>
#include "ITM_write.h"

class Line
{

public:

	Line();
	~Line();
	
};

struct BresenhamD {
	int D;
	int dx;
	int dy;
	int x;
	int y;
	int updateless;
	int updatemore;

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
		init();
	}

	bool update(){
		if(x >= dx){
			return false;
		}else{
			x = x + 1;
			if(D < 0){
				D = D + updateless;
			}else{
				D = D + updatemore;
				y = y + 1;
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