#include "AltitudeSelection.hpp"

// Consructor
AltitudeSelection::AltitudeSelection(){
	_pTimeManagement = new TimeManagement();
}

// Methods which return the amount of degrees to change in function of the velocity of the change
float AltitudeSelection::getDataDifference(){

	float _dataDifference = (float)(_pTimeManagement->getIntervalTimeMillis()/1000);

	// difference between two roll on the right wheel < 0.4 seconds
	// we add/substract 1000 feet
	if(_dataDifference < 0.4){
		_dataDifference = 1000;
	}
	else if(_dataDifference < 1){
		_dataDifference = 500;
	}
	else {
		_dataDifference = 100;
	}

	return _dataDifference;
}
