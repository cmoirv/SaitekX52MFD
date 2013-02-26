#include "TimeManagement.hpp"

//Construtor
TimeManagement::TimeManagement(){

    // get ticks per second
    QueryPerformanceFrequency(&frequency);

	// Initialize the time to compare
    QueryPerformanceCounter(&t1);
}


// return the interval (in milliseconds) of time since the last call
double TimeManagement::getIntervalTimeMillis(){

	double elapsedTime;
    // stop timer
    QueryPerformanceCounter(&t2);

    // compute and print the elapsed time in millisec
    elapsedTime = (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;

	// save current time
	t1 = t2;

	return elapsedTime;
}