#include "HeadingSelection.hpp"

// Consructor
HeadingSelection::HeadingSelection(){
	_pTimeManagement = new TimeManagement();
}

// Methods which return the amount of degrees to change in function of the velocity of the change
float HeadingSelection::getDataDifference(){

	float _dataDifference = (float)(_pTimeManagement->getIntervalTimeMillis()/1000);

	// difference between two roll on the right wheel < 0.4 seconds
	// we add/substract 20 degrees
	if(_dataDifference < 0.4){
		_dataDifference = 20;
	}
	else if(_dataDifference < 1){
		_dataDifference = 5;
	}
	else {
		_dataDifference = 1;
	}

	return _dataDifference;
}
