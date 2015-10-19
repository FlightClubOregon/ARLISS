/*
 * timer.h
 *
 *  Created on: Sep 12, 2015
 *      Author: Parker
 */

#ifndef TIMER_H
#define TIMER_H

using namespace std;

class Timer
{
private:
	char timerName[101];
public:
	Timer();
	Timer(char tempTimep[101]);
	~Timer();
	void getTime();
};




#endif /* ARLISS_TIMER_H_ */
