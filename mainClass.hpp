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
#include <string>
#include <strstream>

//#############################################################################
// Definitionen für diese Klasse
//#############################################################################
enum saDIRECTION { saCOUNT_UP, saCOUNT_DOWN };                       // Richtung, in der der Wert verändert werden soll
enum saSELECT { saSEL_NONE, saSEL_TOPLEFT, saSEL_BOTTOMLEFT_LEFT, 
                saSEL_BOTTOMLEFT_RIGHT, saSEL_TOPRIGHT, 
                saSEL_BOTTOMRIGHT_LEFT, saSEL_BOTTOMRIGHT_RIGHT };   // Bezeichnung für Selektion auf dem MFD
enum saUPDOWNMODE { saUD_SELECT, saUD_VALUE };                       // Markierung oder Selektion (bei Selektion Wert verändern)
enum saCLICKMODE  { saCLICK_CHANGE, saCLICK_VALUE };                 // Wert tauschen oder verändern

//#############################################################################
// Deklaration der Hauptklasse der Anwendung
//#############################################################################
class mainClass
{
private:
  mainClass( void );                                                 // Privat, kann nicht aufgerufen werden
public:
  mainClass( std::string&, std::string&  );                          // Logfilename und Konfig Filename 
  ~mainClass(void);                                                  // der Zerstörer :-)
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
  void softButton( DWORD btns );                                     // Softbutton wird betätigt (Callback)
  void pageChange( DWORD, bool );                                    // Seitenwechsel
  bool setPluginEnabled( bool );                                     // Setze den Status
  bool getPluginEnabled( void );                                     // Ist es erlaubt?
private:
  bool processGearLed( int, saLED );                                 // mach was mit der Gear-LED (falls vorhanden)
  bool processLandingLightLed( int, saLED );                         // Landelicht?
  bool processFlapsLed( int, saLED );                                // mach was mit der FLAPS-LED (falls vorhanden)
  bool processIsRunningLed( int, saLED );                            // Pause LED machen (falls vorhanden)
  bool debug;                                                        // debugging halt...
  float oldGearDeploy[10];                                           // Welcher Fahrwerkstatus wars denn pro Fahrwerk?
  float oldFlapsDeploy;                                              // welcher Status war es denn?
  int oldLandingLightStat;                                           // wie war der Status der Landelichter?
  logThreadClass *plogThreadClass;                                   // Zeiger für die Logging Thread-Klasse
  pluginConfigClass *pPluginConfigClass;                             // Zeiger für die Configurationsklase
  saitekX52ProClass *psaitekX52ProClass;                             // Klasse für Funktionen des Sticks  static float oldGearDeploy[MAXPAGES];                              // Welcher Fahrwerkstatus wars denn?
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
  //XPLMCommandRef mfdCycleUpCmdRef;
  //XPLMCommandRef mfdCycleDownCmdRef;
  saSELECT currCursorSelect;                                         // An welcher Stelle des MFD ist der Cursor?
  saUPDOWNMODE upDownMode;                                           // Selektion oder Markierung?
  saCLICKMODE  clickMode;                                            // Betriebsart schalten oder Frequenz wechseln?
};
