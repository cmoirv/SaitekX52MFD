#include <iostream>
#include <string>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>
#include "mainClass.hpp"

#ifndef DEF_DATA_MANAGEMENT
#define DEF_DATA_MANAGEMENT

// functions to tranform string to be available with #define
#define QUOTE(name) #name
#define STR(macro) QUOTE(macro)

// abstract class to define needed functions
class DataManagement{

public:

	virtual void doMFDDisplay(saitekX52ProClass *psaitekX52ProClass) = 0; 
	virtual void countValue(int dir) = 0;
	//virtual void cycleSelection(int dir) = 0; 
};
#endif