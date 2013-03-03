#include "TimeManagement.hpp"

#ifndef DEF_DATA_SELECTION
#define DEF_DATA_SELECTION

// abstract class to define needed functions to improve selection
class DataSelection{

public:

	// virtual function which return the data difference to add or substract
	virtual float getDataDifference() = 0; 
	
};
#endif