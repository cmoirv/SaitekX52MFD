#include "DataSelection.hpp"
#include "TimeManagement.hpp"

#ifndef DEF_HEADING_SELECTION
#define DEF_HEADING_SELECTION

class HeadingSelection : DataSelection {

public:

	// time management object to get interval of time between two changes of the right wheel
	TimeManagement *_pTimeManagement;
   
	HeadingSelection();

	float getDataDifference(); 
};
#endif