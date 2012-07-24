//#############################################################################
// Deklaration der Klasse zum Speichern der Konfigurationsdate des Plugins
//#############################################################################
#pragma once

#include "projectCommon.h"
#include "projectTypeDefs.hpp"
#include "logThreadClass.hpp"
#include <string>
#include <fstream>
#include <iostream>
#include <vector>

class pluginConfigClass
{
public:
  pluginConfigClass(void);
  pluginConfigClass( std::string& configFile );                      // lese Configuration aus Datei 
  ~pluginConfigClass(void);
  void setLogThreadClass( logThreadClass* _logThreadClass );         // gib mal die Klasse an!
  void readConfig( void );                                           // lese die Datei
  inline bool isDebug(void) { return( debug ); };                    // Debugging?
  inline void setDebug( bool _db ) { debug = _db; };                 // schalten!
  inline saLED getlandingGearLed( void ) { return( landingGear ); }; // welche LED war es noch?
  inline saLED getLandingLightLed( void ) { return( landingLight ); }; // welche LED?
  inline saLED getflapsGearLed( void ) { return( flapsStatus ); };   // welche LED war es noch?
  inline saLED getIsRunningLED( void ) { return( simIsRunning ); };  // Welche LED solls sein?
  inline saLedStat getFlapsColorForStat( saSigStat _st ) { return( flapsLedStats[_st] ); };
  inline saLedStat getGearColorForStat( saSigStat _st ) { return( landingGearLedStats[_st] ); };
  inline saLedStat getLandingLightColorForStat( saSigStat _st ) { return ( landingLightLedStats[_st] ); };
  inline saLedStat getIsRunningColorForStat( saSigStat _st ) { return( simIsRunningLedStats[_st] ); };
  inline bool setPluginEnabled( bool _pe ) { pluginEnabled=_pe; return( pluginEnabled ); };  // Setze den Status
  inline bool getPluginEnabled( void ) {return( pluginEnabled ); };  // Ist es erlaubt?
private:
  bool computeLine( std::string&, std::string&, std::string& );      // bearbeite Input
  bool computeValuePair( std::string&, std::string& );               // setze Paare in config um
  saLED getLEDFromString( std::string& );                            // welche LED ist gemeint?
  std::string sayLED( saLED );                                       // Sag LED als Text
  saColor getColor( std::string& );                                  // Gib Farbwert aus String zurück
  std::string sayColor( saLedStat& );                                // Farbe als String
  void logLine( std::string& );                                      // Hilfsroutine
  void logLine( char* );                                             // Hilfsroutine
  void errorLine( std::string& );                                    // Hilfsroutine
  void errorLine( char* );                                           // Hilfsroutine
  void initDefaults(void);                                           // die Sklavenarbeit
  void overrideSpecials(void);                                       // Nacharbeit
  void printConfigValues(void);                                      // DEBUG Userinfo
  saLedStat ledStat[THROTTLE_AXIS+1];                                // Initialer Status der LED
  logThreadClass* ltClass;                                           // Zeiger auf LogThreadClass
  int blinkInterval;                                                 // Blinkintervall in ms
  bool debug;                                                        // Debug AN oder AUS
  bool pluginEnabled;                                                // Ist das Plugin "enabled"
  std::string configFileName;                                        // wie wollen wir denn heissen?
  saLED landingGear;                                                 // welche LED macht Fahrwerk?
  saLedStat landingGearLedStats[SA_FAILED+1];                        // Zustaende fuer Fahrwerk trans/up/down/fail
  saLED landingLight;                                                // welche LED macht das?
  saLedStat landingLightLedStats[SA_FAILED+1];                       // Zustaende fuer Landing Light on/off
  saLED flapsStatus;                                                 // welche LED fuer Klappen (green=None, red=full, amber=between full and none)
  saLedStat flapsLedStats[SA_FAILED+1];                              // Zustaende fuer Fahrwerk trans/up/down/fail
  saLED simIsRunning;                                                // welche LED macht Pausenanzeige?
  saLedStat simIsRunningLedStats[SA_FAILED+1];                       // Stati running/pausing OK1 OK2
};
