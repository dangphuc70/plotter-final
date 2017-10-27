/*
 * GCodeInterpreter.cpp
 *
 *  Created on: Sep 18, 2017
 *      Author: komar
 */

#include "GCodeInterpreter.h"

UniversalClass::Task GCodeInterpreter::CreateTask(char* line, int len)
{
	UniversalClass::Task t;
	t._Task = UniversalClass::ERROR;
	t._SubTask = UniversalClass::ERROR;
	t._XCordinate = 0;
	t._YCordinate = 0;

	std::string str(line,len); // char to string
	std::string commands[4] = {}; // we can get maximum 4 words as G-command
	int j = 0; // counter how many words there are in the string

	for (int i = 0; i < len; i++)
	{
		if (str[i] != ' ')
		{
			commands[j] = commands[j] + str[i];
		}
		else
		{
			j++;
		}
	}
	j++;
	// check all words and create command from them
	for (int i = 0; i < j; i++)
	{
		if (commands[i][0] == 'X')
		{
			commands[i].erase(0, 1);
			t._XCordinate = std::strtod(commands[i].c_str(),NULL);
		}
		else if (commands[i][0] == 'Y')
		{
			commands[i].erase(0, 1);
			t._YCordinate = std::strtod(commands[i].c_str(),NULL);
		}
		else if ((commands[i][0] == 'A' || commands[i][0] == 'a') && commands[i][1] == '0')
		{
			t._SubTask = UniversalClass::A0;
		}
		else if ((commands[i][0] == 'G' || commands[i][0] == 'g') && commands[i][1] == '1')
		{
			t._Task = UniversalClass::G1;
		}
		else if ((commands[i][0] == 'M'|| commands[i][0] == 'm') && commands[i][1] == '1' && (commands[i].length() > 2) )
		{
			t._Task = UniversalClass::M10;
		}
		else if ((commands[i][0] == 'M'|| commands[i][0] == 'm') && commands[i][1] == '1')
		{
			t._Task = UniversalClass::M1;
		}
		else if ((commands[i][0] == 'M'|| commands[i][0] == 'm') && commands[i][1] == '4')
		{
			t._Task = UniversalClass::M4;
		}
		else if ((commands[i][0] == 'G' || commands[i][0] == 'g') && commands[i][1] == '2')
		{
			t._Task = UniversalClass::G28;
		}
		else if (commands[i][0] == 'R' && commands[i][1] == 'S')
		{
			t._Task = UniversalClass::RESET;
		}
		else if (commands[i][0] == 'D' && commands[i][1] == 'S')
		{
			t._Task = UniversalClass::DISABLE_LIMIT_SAFETY;
		}
		else if (commands[i][0] == 'E' && commands[i][1] == 'N')
		{
			t._Task = UniversalClass::ENABLE_LIMIT_SAFETY;
		}
		else if (commands[i][0] == 'I' && commands[i][1] == 'X')
		{
			t._Task = UniversalClass::IX;
		}
		else if (commands[i][0] == 'I' && commands[i][1] == 'Y')
		{
			t._Task = UniversalClass::IY;
		}
		else if (std::isdigit(commands[i][0]))
		{
			t._SubTask = std::strtod(commands[i].c_str(),NULL);
		}
	}
	return t;
}

void GCodeInterpreter::sendOK()
{
	USB_send((uint8_t *)_Answer, 3);
}
