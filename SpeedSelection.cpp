#include "SpeedSelection.hpp"

// Consructor
SpeedSelection::SpeedSelection(){
	_pTimeManagement = new TimeManagement();
}

// Methods which return the amount of degrees to change in function of the velocity of the change
float SpeedSelection::getDataDifference(){

	float _dataDifference = (float)(_pTimeManagement->getIntervalTimeMillis()/1000);

	// difference between two roll on the right wheel < 0.4 seconds
	// we add/substract 10 knots
	if(_dataDifference < 0.4){
		_dataDifference = 10;
	}
	else if(_dataDifference < 1){
		_dataDifference = 5;
	}
	else {
		_dataDifference = 1;
	}

	return _dataDifference;
}
