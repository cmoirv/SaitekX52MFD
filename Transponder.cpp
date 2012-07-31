
#include "Transponder.hpp"

using namespace std;


Transponder::Transponder()
{
	transponder_code = XPLMFindDataRef("sim/cockpit/radios/transponder_code");
	transponder_mode = XPLMFindDataRef("sim/cockpit/radios/transponder_mode");

	setCode();
	setMode();
}

// Setters
void Transponder::setUnits(int unit)
{
	_units = unit % 8;
	if(_units < 0)
		_units += 8;
	buildAndSetCode();
	
}

void Transponder::setTens(int tens)
{
	_tens=tens % 8;
	if(_tens < 0)
		_tens += 8;
	buildAndSetCode();
}

void Transponder::setHundreds(int hundreds)
{
	_hundreds=hundreds % 8;
	if(_hundreds < 0)
		_hundreds += 8;
	buildAndSetCode();
}

void Transponder::setThousands(int thousands)
{
	_thousands=thousands % 8;
	if(_thousands < 0)
		_thousands += 8;
	buildAndSetCode();
}

void Transponder::setCode()
{
	XPLMSetDatai(transponder_code,_code);
}

void Transponder::setCode(int code)
{
	_code=code;
	setCode();
}

void Transponder::buildAndSetCode()
{
	_code = _units + _tens * 10 + _hundreds * 100 + _thousands * 1000;
	setCode();
}

void Transponder::setMode()
{
	XPLMSetDatai(transponder_mode,_mode);
}

void Transponder::setMode(int mode)
{
	_mode = mode % 5;
	if(_mode < 0)
		_mode += 5;
	setMode();
}

//Getters
int Transponder::getUnits()
{
	getCode();
	return _units;
}

int Transponder::getTens()
{
	getCode();
	return _tens;
}

int Transponder::getHundreds()
{
	getCode();
	return _hundreds;
}

int Transponder::getThousands()
{
	getCode();
	return _thousands;
}

int Transponder::getCode()
{
	_code = XPLMGetDatai(transponder_code);
	_units = _code % 10;
	_tens = ((_code - _units) % 100) / 10;
	_hundreds = ((_code - _units - _tens) % 1000) / 100;
	_thousands = _code / 1000;
	return _code;
}

int Transponder::getMode()
{
	_mode = XPLMGetDatai(transponder_mode);
	return _mode;
}
	


wchar_t * Transponder::getModeString()
{
	getMode();
	switch(_mode)
	{
	case 0:
		return L"OFF";
	break;

	case 1:
		return L"STBY";
	break;

	case 2:
		return L"ON";
	break;

	case 3:
		return L"TEST";
	break;

	case 4:
		return L"MODE 4";
	break;
	default:
		return L"";
	break;
	}
}