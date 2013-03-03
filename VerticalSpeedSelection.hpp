#include "DataSelection.hpp"
#include "TimeManagement.hpp"

#ifndef DEF_VERTICAL_SPEED_SELECTION
#define DEF_VERTICAL_SPEED_SELECTION

class VerticalSpeedSelection : DataSelection {

public:

	// time management object to get interval of time between two changes of the right wheel
	TimeManagement *_pTimeManagement;
   
	VerticalSpeedSelection();

	float getDataDifference(); 
};
#endif