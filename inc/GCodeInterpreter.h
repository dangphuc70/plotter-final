/*
 * GCodeInterpreter.h
 *
 *  Created on: Sep 18, 2017
 *      Author: komar
 */

#ifndef GCODEINTERPRETER_H_
#define GCODEINTERPRETER_H_

#include <string>
#include <cstdlib>
#include <stdlib.h>
#include "user_vcom.h"
#include "UniversalClass.h"

class GCodeInterpreter {

public:
	UniversalClass::Task CreateTask(char* line, int len);
	void sendOK();
private:
	const char _Answer[4]="OK\n";
};

#endif /* GCODEINTERPRETER_H_ */
