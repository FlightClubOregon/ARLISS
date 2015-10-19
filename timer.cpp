/*
 * timer.cpp
 *
 *  Created on: Sep 12, 2015
 *      Author: Parker
 */
#include <time.h>
#include <iostream>
#include <cstring>
#include <fstream>
#include "timer.h"
#include <sys/time.h>
#include <time.h>
#include <stdlib.h>
#define _POSIX_SOURCE
#include <time.h>

Timer::Timer()
{
	strcpy(timerName, "no name");
}

Timer::Timer(char tempTimer[101])
{
	strcpy(timerName, tempTimer);
}


Timer::~Timer()
{

}
void Timer::getTime()
{
	ofstream outData;

/*	 time_t rawtime;
	 struct tm * timeinfo;
	 time (&rawtime);
	 timeinfo = localtime (&rawtime);
	 outData << asctime(timeinfo)<<endl;
	 outData.close();
*/

/*	const long double sysTime = time(0);
	const long double sysTimeMS = sysTime*1000;

	outData << sysTimeMS << endl;
	outData.close();

	cout << "Current system time is: " << sysTimeMS << endl;
	cout << endl;*/

	char buffer[30];
	  struct timeval tv;

	  time_t curtime;



	  gettimeofday(&tv, NULL);
	  curtime=tv.tv_sec;

	  strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));

	  outData.open("time.txt", std::ios::out |std::ios::app);
	  outData << buffer << tv.tv_usec << endl;
	  outData.close();

}




