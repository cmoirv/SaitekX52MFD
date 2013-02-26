//#############################################################################
// Deklaration der Klasse f�r den Stick
//#############################################################################
#pragma once

#include "projectCommon.h"
#include "projectTypeDefs.hpp"
#include "pluginConfigClass.hpp"
#include "logThreadClass.hpp"
#include "mfdException.hpp"
#include <DirectOutput.h>
#include <DirectOutputHelper.h>
#include <iostream>
#include <sstream>
#include <string>

//#define STUECK_MAXPAGES 3                                            // Anzahl der Seiten, die ich zulasse
#define STUECK_MAXPAGES 8

// ids of different pages, it will be easy to change the order
#define ID_PAGE_SPEEDS		0
#define ID_PAGE_COM1NAV1	1
#define ID_PAGE_COM2NAV2	2
#define ID_PAGE_NAV1		3
#define ID_PAGE_NAV2		4
#define ID_PAGE_QNH			6
#define ID_PAGE_TRANSPONDER	7
#define ID_PAGE_AUTOPILOT	5



typedef void ( __stdcall dmSoftButtonCallback)(IN DWORD btnd, IN void* pCtxt); // Typendefinition f�r meinen Callback
typedef void ( __stdcall dmPageChangeCallback)(IN DWORD page, IN bool active, IN void* pCtxt); // Typendefinition f�r neuen Callback

//#############################################################################
// Deklaration der Klasse f�r den Stick
//#############################################################################
class saitekX52ProClass: public CDirectOutput
{
public:
  saitekX52ProClass( void );                                         // Der Standartkonstruktor
  saitekX52ProClass( logThreadClass * );                             // Konstruktor mit Logging-Thread Objekt
  ~saitekX52ProClass( void );                                        // Der Zerst�rer         
  void setLogObject( logThreadClass * );                             // setze nachtr�glich die LogThrad Klasse
  bool stickFuncInit( IN const std::wstring& );                      // Initialisiere alle wichtigen Funktionen
  bool stickFuncDeinit( void );                                      // Deinitialisiere alle Funktionen (Direct Input)
  void deviceChange( void* , bool );                                 // Callbackfunktion, wenn Ger�t gewechselt wird
  void pageChange( void* , DWORD , bool  );                          // Callbackfunktion, wenn die Anzeigeseite gewechselt wird
  void softButton( void* , DWORD  );                                 // Callbackfunktion, wenn ein "softbutton" bet�tigt wird
  bool registerSoftButtonCalback( dmSoftButtonCallback *, void * );  // Neue Callbackfunktion registrieren
  bool registerPageChangeCallback( dmPageChangeCallback *, void *);  // neue Callbackfunktion registrieren
  //void* getDevice( int dev );
  int addPage( int , std::wstring& , bool );                         // F�ge eine Seite auf dem MFD hinzu
  int delPage( int index );                                          // entferne eine Seite vom MFD
  bool delAllPages( void );                                          // entferne alle Seiten vom MFD
  int getActivePage( void );                                         // gib mir die aktive Seitennummer 
  int setActivePage( int );                                          // setze die aktive Seite
  bool setString( int, int, std::wstring& );                         // setze einen String auf Position Seite,Zeile
  int setLed( int, saLED, saColor, saBlink );                        // Setze eine LED mit einer Farbe, blinkend oder nichtblinkend
  saLedStat& getLedStat( saLED );                                    // Gib eine referenz auf Status von LED
  bool makeAllLedBlink( bool );                                      // ale LED auf Blinken pruefen, bool=true setzt auf jeden fall 
  void setLedStat( saLED, saLedStat& );                              // setze den Status ohne weitere Ma�nahme
  bool logLine( std::string& );                                      // schreibe eine Zeile in das LOG
  bool logLine( char * );                                            // schreibe eine Zeile in das LOG
  bool errorLine( std::string& );                                    // schreibe eine Fehlerzeile ins LOG
  bool errorLine( char * );                                          // schreibe eine Fehlezeile ins LOG
  bool setDebug( bool );                                             // Setze debugging, gib alten Wert zur�ck
  bool getDebug( void );                                             // erfrage den debug-Zustand
private:
  void getLedId( saLED, int&, int& );                                // gib die ID der entsprechenden LED zur�ck
  void getVColors( saColor, int&, int& );                            // gibt on/off fuer die Farben zur�ck
  logThreadClass *pLogThreadClass;                                   // Speicher f�r das Log Objekt
  void *gDevice;                                                     // Zeiger auf SAITEK Device
  int pages[STUECK_MAXPAGES];                                        // Array f�r MFD Seiten
  int activePage;                                                    // Welche Seite ist aktiv?
  dmSoftButtonCallback *softButtonCb;                                // Speicher f�r eigene Callback-Routine
  void *softButtonCtx;                                               // Zeiger
  dmPageChangeCallback *pageChangeCb;                                // Speicher f�r eigene Callback-Routine
  void *pageChangeCtx;                                               // Zeiger
  saLedStat ledStat[THROTTLE_AXIS];                                  // Array f�r den Zustand der LED
  std::stringstream nLine;                                           // Stringstream zum formatieren
  bool debug;                                                        // Speicher f�r debug Zustand
  bool onPhase;                                                      // Blink: On oder off
};
