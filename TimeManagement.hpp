#include <time.h>
#include <windows.h>// for Windows APIs

#ifndef DEF_TIME_MANAGEMENT
#define DEF_TIME_MANAGEMENT

class TimeManagement{

public:

	// Variables to allow faster selection
	LARGE_INTEGER frequency;        // ticks per second
    LARGE_INTEGER t1, t2;           // ticks
   
	TimeManagement();

	double getIntervalTimeMillis();
};
#endif