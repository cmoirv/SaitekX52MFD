#include <iostream>
#include <string>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>
#include "mainClass.hpp"

#ifndef DEF_DATA_MANAGEMENT
#define DEF_DATA_MANAGEMENT
// abstract class to define needed functions
class DataManagement{

public:
	
	virtual float doMFDDisplay( void ) = 0; 
	//virtual void countValue( int  ) = 0;
	virtual void cycleSelection( int dir  ) = 0; 
};
#endif