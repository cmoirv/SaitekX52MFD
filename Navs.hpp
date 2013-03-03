#include "DataManagement.hpp"
#include "HeadingSelection.hpp"

#ifndef DEF_NAVS
#define DEF_NAVS

//definition of dataref
#define OBS_NAV1_NAME sim/cockpit/radios/nav1_obs_degm
#define OBS_NAV2_NAME sim/cockpit/radios/nav2_obs_degm

#define DME_NAV1_NAME sim/cockpit/radios/nav1_dme_dist_m
#define DME_NAV2_NAME sim/cockpit/radios/nav2_dme_dist_m

#define NAV1_FRQ_NAME sim/cockpit/radios/nav1_freq_hz
#define NAV2_FRQ_NAME sim/cockpit/radios/nav2_freq_hz

// definition of variable which will be used
#define OBS_NAV1 STR(OBS_NAV1_NAME) 
#define OBS_NAV2 STR(OBS_NAV2_NAME) 

#define DME_NAV1 STR(DME_NAV1_NAME) 
#define DME_NAV2 STR(DME_NAV2_NAME)

#define NAV1_FRQ STR(NAV1_FRQ_NAME)
#define NAV2_FRQ STR(NAV2_FRQ_NAME)

enum saNAV {NAV1, NAV2}; // allow to choose which nav we want

// Navs Class to manage Navs Data
class Navs {

private:
	// DataRef used by the NAV page
	XPLMDataRef _obs_nav;
	XPLMDataRef _dme;
	XPLMDataRef _navFreq;

	// id of nav (nav 1 or 2)
	saNAV _idNav;

	HeadingSelection *_pHeadingSelection;

public:
	
	//Navs();
	Navs(saNAV);
	
	void doMFDDisplay( saitekX52ProClass *psaitekX52ProClass ); 
	void countValue(int dir);
};

#endif