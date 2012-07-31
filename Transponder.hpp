#include <iostream>
#include <string>
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>

#define TRANSPONDER_OFF 0
#define TRANSPONDER_STBY 1
#define TRANSPONDER_ON 2
#define TRANSPONDER_TEST 3
#define TRANSPONDER_MODE_4 4

class Transponder
{
private :
	int _units, _tens, _hundreds, _thousands;
	int _code;
	int _mode;

	XPLMDataRef transponder_code;
	XPLMDataRef transponder_mode;

public :

	Transponder();
	void setUnits(int unit);
	void setTens(int tens);
	void setHundreds(int hundreds);
	void setThousands(int thousands);
	void setCode(int code);
	void setCode();
	void buildAndSetCode();
	void setMode();
	void setMode(int mode);


	int getUnits();
	int getTens();
	int getHundreds();
	int getThousands();
	int getCode();
	int getMode();
	wchar_t * getModeString();

};