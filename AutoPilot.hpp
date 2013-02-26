#include "DataManagement.hpp"

#ifndef DEF_AUTOPILOT
#define DEF_AUTOPILOT

//definition of dataref
#define AP_MODE_NAME			sim/cockpit/autopilot/autopilot_mode
#define AP_ALTITUDE_NAME		sim/cockpit/autopilot/altitude
#define AP_VERTICAL_SPEED_NAME	sim/cockpit/autopilot/vertical_velocity
#define AP_SPEED_NAME			sim/cockpit/autopilot/airspeed
#define AP_HEADING_NAME			sim/cockpit/autopilot/heading_mag

// definition of variable which will be used
#define AP_MODE				STR(AP_MODE_NAME) 
#define AP_ALTITUDE			STR(AP_ALTITUDE_NAME)
#define AP_VERTICAL_SPEED	STR(AP_VERTICAL_SPEED_NAME)
#define AP_SPEED			STR(AP_SPEED_NAME)
#define AP_HEADING			STR(AP_HEADING_NAME)

// Class definition
class AutoPilot : DataManagement {

	/*
	AP[OFF] VS[XXXX]
	ALT[XXXXX]
	SPD:XXX HDG:XXX]
	*/

private :
	// indexes for displaying different pattern on each line 
	int _indexDisplayLine1;
	int _indexDisplayLine2;
	int _indexDisplayLine3;

	// id of selection
	int _currCursorSelect;

	// DataRef used by the AutoPilot page
	XPLMDataRef _mode;
	XPLMDataRef _vertical_speed;
	XPLMDataRef _altitude;
	XPLMDataRef _speed;
	XPLMDataRef _heading;

public:
	AutoPilot();
	void doMFDDisplay( saitekX52ProClass *psaitekX52ProClass ); 
	void countValue(int dir);
};
#endif