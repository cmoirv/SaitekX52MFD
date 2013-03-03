#include "AutoPilot.hpp"

wchar_t *displayFormatAPLine1[3] =
{
	L"AP:%s VS:%5.0f\0",
	L"AP[%s]VS:%5.0f\0",
	L"AP:%s VS[%5.0f]\0"
};
wchar_t *displayFormatAPLine2[2] =
{
	L"ALT:%5.0f  HDG\0",
	L"ALT[%5.0f] HDG\0"
};

wchar_t *displayFormatAPLine3[6] =
{
	L"SPD:%3.0f    %03.0f \0",
	L"SPD[%3.0f]   %03.0f \0",
	L"SPD:%3.0f   [%03.0f] \0",
	L"SPD:%.2f   %03.0f \0",
	L"SPD[%.2f]  %03.0f \0",
	L"SPD:%.2f  [%03.0f] \0"
};

wchar_t *displayFormatAPMode[3] =
{
	L"OFF",
	L"FD ",
	L"ON "
};


// Constructor
AutoPilot::AutoPilot(){
	int _indexDisplayLine1 = 0;
	int _indexDisplayLine2 = 0;
	int _indexDisplayLine3 = 0;

	// Initialisation of dataref
	_mode = XPLMFindDataRef(AP_MODE);
	_vertical_speed = XPLMFindDataRef(AP_VERTICAL_SPEED);
	_altitude		= XPLMFindDataRef(AP_ALTITUDE);
	_speed			= XPLMFindDataRef(AP_SPEED);
	_heading		= XPLMFindDataRef(AP_HEADING);
	_isMachSpeed	= XPLMFindDataRef(AP_ISMACHSPEED);

	// instanciation of objects for selection
	_pHeadingSelection		= new HeadingSelection();
	_pSpeedSelection		= new SpeedSelection();
	_pAltitudeSelection		= new AltitudeSelection();
	_pVerticalSpeedSelection= new VerticalSpeedSelection();	
}

//#############################################################################
// Chage value in function of the selection
//#############################################################################
void AutoPilot::countValue(int dir){

	// number used to divide the result of speed
	// to managed mach speed
	int _divSpeed = 1;

	switch( _currCursorSelect )
	{
	case saSEL_AP_MODE: // change AP mode
		if( dir == saCOUNT_UP )
		{
			XPLMSetDatai(_mode, XPLMGetDatai(_mode) + 1);
			if(XPLMGetDatai(_mode) > 2)
			{
				XPLMSetDatai(_mode, 0);
			}
		}
		else
		{
			XPLMSetDatai(_mode, XPLMGetDatai(_mode) - 1);
			if(XPLMGetDatai(_mode) < 0)
			{
				XPLMSetDatai(_mode, 2);
			}
		}
		break;

	case saSEL_AP_VERTICAL_SPEED : 
		if( dir == saCOUNT_UP )
		{
			XPLMSetDataf(_vertical_speed, XPLMGetDataf(_vertical_speed) + _pVerticalSpeedSelection->getDataDifference());
		}
		else
		{
			XPLMSetDataf(_vertical_speed, XPLMGetDataf(_vertical_speed) - _pVerticalSpeedSelection->getDataDifference());
		}
		break;

	case saSEL_AP_ALTITUDE :
		if( dir == saCOUNT_UP )
		{
			XPLMSetDataf(_altitude, (int)(XPLMGetDataf(_altitude)/100)*100 + _pAltitudeSelection->getDataDifference());
		}
		else
		{
			XPLMSetDataf(_altitude, (int)(XPLMGetDataf(_altitude)/100)*100 - _pAltitudeSelection->getDataDifference());
			if(XPLMGetDataf(_altitude) <= -1000)
			{ 
				XPLMSetDataf(_altitude, -1000);
			}
		}
		break;

	case saSEL_AP_SPEED :

		// change pattern to display mach number
		if(XPLMGetDatai(_isMachSpeed) == 1){
			// divide by 100 if the selection is a mach number
			_divSpeed = 100;
		}
		else 
		{
			_divSpeed = 1;
		}

		if( dir == saCOUNT_UP )
		{
			XPLMSetDataf(_speed, XPLMGetDataf(_speed) + (_pSpeedSelection->getDataDifference()/_divSpeed));
		}
		else
		{
			XPLMSetDataf(_speed, XPLMGetDataf(_speed) - (_pSpeedSelection->getDataDifference()/_divSpeed));
			if(XPLMGetDataf(_speed) <= 0)
			{ 
				XPLMSetDataf(_speed, 0.0f);
			}
		}
		break;

	case saSEL_AP_HEADING :
		if( dir == saCOUNT_UP )
		{
			XPLMSetDataf(_heading, XPLMGetDataf(_heading) + _pHeadingSelection->getDataDifference());
			if(XPLMGetDataf(_heading) >= 361)
			{
				XPLMSetDataf(_heading, XPLMGetDataf(_heading) - 360.0f);
			}
		}
		else
		{
			XPLMSetDataf(_heading, XPLMGetDataf(_heading) - _pHeadingSelection->getDataDifference());
			if(XPLMGetDataf(_heading) <= 1)
			{ 
				XPLMSetDataf(_heading, XPLMGetDataf(_heading) + 360.0f);
			}
		}
		break;
	}
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

	// change pattern to display mach number
	if(XPLMGetDatai(_isMachSpeed) == 1){
		_indexDisplayLine3 +=3;
	}

	// First line
	swprintf( (wchar_t *)&wbuffer[0], 17, displayFormatAPLine1[_indexDisplayLine1], displayFormatAPMode[XPLMGetDatai(_mode)], XPLMGetDataf(_vertical_speed));
	psaitekX52ProClass->setString( activePage, 0, std::wstring( (wchar_t *)&wbuffer[0] ) );

	// Second line
	swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatAPLine2[_indexDisplayLine2], XPLMGetDataf(_altitude));
	psaitekX52ProClass->setString( activePage, 1, std::wstring( (wchar_t *)&wbuffer[0] ) );

	// Third line
	swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatAPLine3[_indexDisplayLine3], XPLMGetDataf(_speed), XPLMGetDataf(_heading));
	psaitekX52ProClass->setString( activePage, 2, std::wstring( (wchar_t *)&wbuffer[0] ) );
}

int AutoPilot::cycleSelection( int dir, int currCursorSelect){
	switch( currCursorSelect ) 
	{
	case saSEL_NONE: 

		if( dir == saCOUNT_UP )
			currCursorSelect = saSEL_AP_HEADING;
		else
			currCursorSelect = saSEL_AP_MODE;
		break;
	case saSEL_AP_MODE: // selection of the autopilot mode
		if( dir == saCOUNT_UP ) 
			currCursorSelect = saSEL_NONE;

		else 
			currCursorSelect = saSEL_AP_VERTICAL_SPEED;
		break;
	case saSEL_AP_VERTICAL_SPEED: 
		if( dir == saCOUNT_UP ) 
			currCursorSelect = saSEL_AP_MODE;
		else
			currCursorSelect = saSEL_AP_ALTITUDE;

		break;
	case saSEL_AP_ALTITUDE: 
		if( dir == saCOUNT_UP )
			currCursorSelect = saSEL_AP_VERTICAL_SPEED;
		else
			currCursorSelect = saSEL_AP_SPEED;
		break;
	case saSEL_AP_SPEED: 
		if( dir == saCOUNT_UP )

			currCursorSelect = saSEL_AP_ALTITUDE;
		else
			currCursorSelect = saSEL_AP_HEADING;
		break;
	case saSEL_AP_HEADING: 
		if( dir == saCOUNT_UP )
			currCursorSelect = saSEL_AP_SPEED;
		else
			currCursorSelect = saSEL_NONE;

		break;
	}
	_currCursorSelect = currCursorSelect;

	return currCursorSelect;
}