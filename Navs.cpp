#include "Navs.hpp"

wchar_t *displayFormatDiffNav[1] =
{
	L" NAV%d : %03d.%02d\0"
};

// Constructor to set dataRef
Navs::Navs(saNAV nav){

	_idNav = nav;
	_obs_deg_amount = 20;

	_pTimeManagement = new TimeManagement();

	if(nav == NAV1){

		_obs_nav = XPLMFindDataRef(OBS_NAV1);
		_dme = XPLMFindDataRef(DME_NAV1);
		_navFreq = XPLMFindDataRef(NAV1_FRQ);

	}
	else{
		_obs_nav = XPLMFindDataRef(OBS_NAV2);
		_dme = XPLMFindDataRef(DME_NAV2);
		_navFreq = XPLMFindDataRef(NAV2_FRQ);
	}
}



//#############################################################################
// OBS selection for NAVs
//#############################################################################
void Navs::countValue(int dir){

	_obs_deg_amount = (float)(_pTimeManagement->getIntervalTimeMillis()/1000);

	if(_obs_deg_amount < 0.4){
		_obs_deg_amount = 20;
	}
	else if(_obs_deg_amount < 1){
		_obs_deg_amount = 5;
	}
	else {
		_obs_deg_amount = 1;
	}

	if( dir == saCOUNT_UP )
	{
		XPLMSetDataf(_obs_nav, XPLMGetDataf(_obs_nav) + _obs_deg_amount);
		if(XPLMGetDataf(_obs_nav) >= 361)
		{
			XPLMSetDataf(_obs_nav, XPLMGetDataf(_obs_nav) - 360.0f);
		}
	}
	else
	{
		XPLMSetDataf(_obs_nav, XPLMGetDataf(_obs_nav) - _obs_deg_amount);
		if(XPLMGetDataf(_obs_nav) <= 1)
		{ 
			XPLMSetDataf(_obs_nav, XPLMGetDataf(_obs_nav) + 360.0f);
		}
	}
}

// Manage display data
void Navs::doMFDDisplay( saitekX52ProClass *psaitekX52ProClass ){

	char wbuffer[36]; 
	int activePage = psaitekX52ProClass->getActivePage();

	if(_idNav == NAV1){
		swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatDiffNav[0], 1, XPLMGetDatai(_navFreq)/100, XPLMGetDatai(_navFreq)%100);
	}
	else{
		swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatDiffNav[0], 2, XPLMGetDatai(_navFreq)/100, XPLMGetDatai(_navFreq)%100);
	}
	psaitekX52ProClass->setString( activePage, 0, std::wstring( (wchar_t *)&wbuffer[0] ) );
	swprintf( (wchar_t *)&wbuffer[0], 16, L"  OBS : %03.0f \0", XPLMGetDataf(_obs_nav));
	psaitekX52ProClass->setString( activePage, 1, std::wstring( (wchar_t *)&wbuffer[0] ) );
	swprintf( (wchar_t *)&wbuffer[0], 16, L"  DME %03.1f NM\0", XPLMGetDataf( _dme ));
	psaitekX52ProClass->setString( activePage, 2, std::wstring( (wchar_t *)&wbuffer[0] ) );
}