#include "AutoPilot.hpp"

wchar_t *displayFormatAPLine1[3] =
{
	L"AP:%s VS:%5.0f\0",
	L"AP[%s]VS:%5.0f\0",
	L"AP:%s VS[%5.0f]\0"
};
wchar_t *displayFormatAPLine2[2] =
{
	L"ALT:%5.0f\0",
	L"ALT[%5.0f]\0"
};

wchar_t *displayFormatAPLine3[3] =
{
	L"SPD:%3.0f HDG:%03.0f\0",
	L"SPD[%3.0f]HDG:%03.0f\0",
	L"SPD:%3.0f HDG[%03.0f]\0"
};

wchar_t *displayFormatAPMode[3] =
{
	L"OFF\0",
	L"FD \0",
	L"ON \0"
};


// Constructor
AutoPilot::AutoPilot(){
	int _indexDisplayLine1 = 0;
	int _indexDisplayLine2 = 0;
	int _indexDisplayLine3 = 0;

	// Initialisation of dataref
	_mode = XPLMFindDataRef(AP_MODE);
	_vertical_speed = XPLMFindDataRef(AP_VERTICAL_SPEED);
	_altitude = XPLMFindDataRef(AP_ALTITUDE);
	_speed = XPLMFindDataRef(AP_SPEED);
	_heading = XPLMFindDataRef(AP_HEADING);
}

//#############################################################################
// Chage value in function of the selection
//#############################################################################
void AutoPilot::countValue(int dir){

	/*	_obs_deg_amount = (float)(_pTimeManagement->getIntervalTimeMillis()/1000);

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
	}*/
}

// Manage display data
void AutoPilot::doMFDDisplay( saitekX52ProClass *psaitekX52ProClass ){

	switch( _currCursorSelect )
	{

	case saSEL_NONE: 
		_indexDisplayLine1=0;
		_indexDisplayLine2=0;
		_indexDisplayLine3=0;
		break;

	case saSEL_AP_MODE: 
		_indexDisplayLine1=1;
		_indexDisplayLine2=0;
		_indexDisplayLine3=0;
		break;

	case saSEL_AP_VERTICAL_SPEED : 
		_indexDisplayLine1=2;
		_indexDisplayLine2=0;
		_indexDisplayLine3=0;
		break;

	case saSEL_AP_ALTITUDE :
		_indexDisplayLine1=0;
		_indexDisplayLine2=1;
		_indexDisplayLine3=0;
		break;

	case saSEL_AP_SPEED :
		_indexDisplayLine1=0;
		_indexDisplayLine2=0;
		_indexDisplayLine3=1;
		break;

	case saSEL_AP_HEADING :
		_indexDisplayLine1=0;
		_indexDisplayLine2=0;
		_indexDisplayLine3=2;
		break;
	}

	char wbuffer[36]; 
	int activePage = psaitekX52ProClass->getActivePage();

/*	int _Imode = XPLMGetDatai(_mode);
	char _modeChar[4];

	switch(_Imode){
	case 0:
		sprintf_s(_modeChar, 4, "OFF\0");
		break;
	case 1:
		sprintf_s(_modeChar, 4,"FD \0");
		break;
	case 2:
		sprintf_s(_modeChar, 4,"ON \0");
		break;
	}*/

	// First line
	swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatAPLine1[_indexDisplayLine1], displayFormatAPMode[XPLMGetDatai(_mode)], XPLMGetDataf(_vertical_speed));
	psaitekX52ProClass->setString( activePage, 0, std::wstring( (wchar_t *)&wbuffer[0] ) );

	// Second line
	swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatAPLine2[_indexDisplayLine2], XPLMGetDataf(_altitude));
	psaitekX52ProClass->setString( activePage, 1, std::wstring( (wchar_t *)&wbuffer[0] ) );

	// Third line
	swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatAPLine3[_indexDisplayLine3], XPLMGetDataf(_speed), XPLMGetDataf(_heading));
	psaitekX52ProClass->setString( activePage, 2, std::wstring( (wchar_t *)&wbuffer[0] ) );
}