#include "DataSelection.hpp"
#include "TimeManagement.hpp"

#ifndef DEF_SPEED_SELECTION
#define DEF_SPEED_SELECTION

class SpeedSelection : DataSelection {

public:

	// time management object to get interval of time between two changes of the right wheel
	TimeManagement *_pTimeManagement;
   
	SpeedSelection();

	float getDataDifference(); 
};
#endif