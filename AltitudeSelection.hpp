#include "DataSelection.hpp"
#include "TimeManagement.hpp"

#ifndef DEF_ALTITUDE_SELECTION
#define DEF_ALTITUDE_SELECTION

class AltitudeSelection : DataSelection {

public:

	// time management object to get interval of time between two changes of the right wheel
	TimeManagement *_pTimeManagement;
   
	AltitudeSelection();

	float getDataDifference(); 
};
#endif