//#############################################################################
// Definitionen für die Main Source
// Interface Definitionen für das Interface zu X-Plane
//#############################################################################
#pragma once

#include "projectCommon.h"
#include "pluginConfigClass.hpp"
#include "mainClass.hpp"
#include <XPLMDataAccess.h>
//#include <XPLMMenus.h>
#include <XPLMProcessing.h>
#include <XPLMUtilities.h>

#ifdef __cplusplus
extern "C" {
#endif

extern mainClass *pmCl;                                          // Zeiger auf Hauptklasse, die das alles Managt. GLOBAL

// Plugin Funktionen Definition für DLL-Export
// die Funktionen ruft die Plugin-Schnittstelle auf
PLUGIN_API int XPluginStart( char *outName, char *outSig, char *outDesc );
PLUGIN_API void	XPluginStop(void);
PLUGIN_API void XPluginDisable(void);
PLUGIN_API int XPluginEnable(void);
PLUGIN_API void XPluginReceiveMessage( XPLMPluginID inFromWho, long inMessage, void *inParam );

#ifdef __cplusplus
};
#endif

// Callbackfunktion für Menü
//PLUGIN_API void	myMenuHandlerCallback( void *inMenuRef, void *inItemRef );
// Callback timergesteuert
PLUGIN_API float myfLoopCallback( float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void  *inRefcon);    
// Aufruf durch Plugin Intercace
PLUGIN_API int initProgrammData( char *logFile );
PLUGIN_API int freeProgrammData(void);
PLUGIN_API void enablePlugin(void);
PLUGIN_API void disablePlugin(void);


