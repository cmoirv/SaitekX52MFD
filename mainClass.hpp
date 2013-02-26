//#############################################################################
// Deklaration der Hauptklasse des Projektes
//#############################################################################
#pragma once

#include "projectCommon.h"
#include "pluginConfigClass.hpp"
#include <XPLMDataAccess.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>
#include "logThreadClass.hpp"
#include "saitekX52ProClass.hpp"
#include "Transponder.hpp"
#include <string>
#include <strstream>
#include "Navs.hpp"
#include "AutoPilot.hpp"

//#############################################################################
// Definitionen f�r diese Klasse
//#############################################################################
enum saDIRECTION { saCOUNT_UP, saCOUNT_DOWN };                       // Richtung, in der der Wert ver�ndert werden soll
typedef enum saSELECT { saSEL_NONE, saSEL_TOPLEFT, saSEL_BOTTOMLEFT_LEFT, 
	saSEL_BOTTOMLEFT_RIGHT, saSEL_TOPRIGHT, 
	saSEL_BOTTOMRIGHT_LEFT, saSEL_BOTTOMRIGHT_RIGHT, 
	saSEL_TPR_THDS, saSEL_TPR_HDS, saSEL_TPR_TENS, saSEL_TPR_UNTS, saSEL_TPR_MODE,
	saSEL_AP_MODE, saSEL_AP_VERTICAL_SPEED, saSEL_AP_ALTITUDE, saSEL_AP_SPEED, saSEL_AP_HEADING} esaSELECT;   // Bezeichnung f�r Selektion auf dem MFD
enum saUPDOWNMODE { saUD_SELECT, saUD_VALUE };                       // Markierung oder Selektion (bei Selektion Wert ver�ndern)
enum saCLICKMODE  { saCLICK_CHANGE, saCLICK_VALUE };                 // Wert tauschen oder ver�ndern

//#############################################################################
// Deklaration der Hauptklasse der Anwendung
//#############################################################################
class mainClass
{
private:
	mainClass( void );                                                 // Privat, kann nicht aufgerufen werden
public:
	mainClass( std::string&, std::string&  );                          // Logfilename und Konfig Filename 
	~mainClass(void);                                                  // der Zerst�rer :-)
	bool initProgrammData( void );                                     // Initialisiere die DirectOutput Software
	float doMFDDisplay( void );                                        // das Teil macht zyklisch die Anzeige neu
	float doLEDStatus( float inElapsedSinceLastCall );                 // die LED neu setzen...
	//int computeCommands( XPLMCommandRef cmd, XPLMCommandPhase phase, void *arg ); // Wenn Kommandos vom Simulator kommen
	bool logLine( std::string& line );                                 // wie der Name schonsagt...
	bool logLine( char *line );
	bool errorLine( std::string& line );
	bool errorLine( char *line );
	void flushStream( void );                                          // Schreiben in die Datei, Puffer leeren
	void countValue( saDIRECTION );
	void cycleSelection( saDIRECTION );                                // Selektion auf dem Display weiterschalten
	void changeFrequence( void );                                      // Wechsel der Com/Nav Frequenz aktiv <-> standby
	//inline void setMfdCycleUpCmdRef( XPLMCommandRef cmdRef ) { mfdCycleUpCmdRef = cmdRef; };
	//inline void setMfdCycleDownCmdRef( XPLMCommandRef cmdRef ) { mfdCycleDownCmdRef = cmdRef; };
	void softButton( DWORD btns );                                     // Softbutton wird bet�tigt (Callback)
	void pageChange( DWORD, bool );                                    // Seitenwechsel
private:
	bool processGearLed( int, saLED );                                 // mach was mit der Gear-LED (falls vorhanden)
	bool processFlapsLed( int, saLED );                                // mach was mit der FLAPS-LED (falls vorhanden)
	bool processIsRunningLed( int, saLED );                            // Pause LED machen (falls vorhanden)
	bool debug;                                                        // debugging halt...
	float oldGearDeploy[10];                                           // Welcher Fahrwerkstatus wars denn pro Fahrwerk?
	float oldFlapsDeploy;                                              // welcher Status war es denn?
	logThreadClass *plogThreadClass;                                   // Zeiger f�r die Logging Thread-Klasse
	pluginConfigClass *pPluginConfigClass;                             // Zeiger f�r die Configurationsklase
	saitekX52ProClass *psaitekX52ProClass;                             // Klasse f�r Funktionen des Sticks  static float oldGearDeploy[MAXPAGES];                              // Welcher Fahrwerkstatus wars denn?
	Transponder *transponder;

	// Object for Nav 1 and Nav 2
	Navs		*nav1;
	Navs		*nav2;

	// Object for autopilot
	AutoPilot	*autopilot;

	XPLMDataRef groundSpeedRef;                                        // Referenz en vom Simulator
	XPLMDataRef indicatedAirSpeedRef;
	XPLMDataRef verticalSpeedRef;
	XPLMDataRef elevationRev;
	XPLMDataRef indicatedBaroElevRef;
	XPLMDataRef com1Ref;
	XPLMDataRef com2Ref;
	XPLMDataRef com1StbyRef;
	XPLMDataRef com2StbyRef;
	XPLMDataRef nav1Ref;
	XPLMDataRef nav2Ref;
	XPLMDataRef nav1StbyRef;
	XPLMDataRef nav2StbyRef;
	XPLMDataRef gearRetract;                                           // Fahrwerk in Aktion =>acf_gear_retract
	XPLMDataRef gearDeploy;                                            // Fahrwerkstatus 0.0 -> 1.0 => acf_gear_deploy
	XPLMDataRef gearIsSkid;                                            // festes Fahrwerk int
	XPLMDataRef flapsDeploy;                                           // Klappen-Status...
	XPLMDataRef landingLightsOn;                                       // Ist das Landelicht AN?
	XPLMDataRef simIsPaused;                                           // Pause oder nicht? int
	// Add HLM
	XPLMDataRef barometer_setting;
	// End add HLM
	//XPLMCommandRef mfdCycleUpCmdRef;
	//XPLMCommandRef mfdCycleDownCmdRef;
	saSELECT currCursorSelect;                                         // An welcher Stelle des MFD ist der Cursor?
	saUPDOWNMODE upDownMode;                                           // Selektion oder Markierung?
	saCLICKMODE  clickMode;                                            // Betriebsart schalten oder Frequenz wechseln?
};
