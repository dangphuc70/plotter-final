/*
 * UniversalClass.h
 *
 *  Created on: Sep 18, 2017
 *      Author: komar
 */

#ifndef UNIVERSALCLASS_H_
#define UNIVERSALCLASS_H_

class UniversalClass {

public:
enum tasks
{
	ERROR = -2,
	A0 = 0, // why not
	G1 = 1,
	G28 = 2,
	M1 = 3,
	M4 = 4,
	M10 = 5,
	RESET = 6
};
struct Task
{
	tasks _Task;
	int _SubTask;
	double _XCordinate;
	double _YCordinate;
}task;


};

#endif /* UNIVERSALCLASS_H_ */
