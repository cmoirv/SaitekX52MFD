//#############################################################################
// Und hier beginnt es...
//#############################################################################
#include "projectCommon.h"
#include "mainClass.hpp"
#include <string>

mainClass *pmCl = 0;                                                  // Zeiger auf Hauptklasse, die das alles Managt.
PLUGIN_API int dispatchCommand( XPLMCommandRef cmd, XPLMCommandPhase phase, void *arg );

using namespace std;

//#############################################################################
// die DLL Eintrittsprozedur, hier nicht weiter benutzt
//#############################################################################
BOOL APIENTRY DllMain( HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved )
{
  switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:
			break;
  }
  return TRUE;
}

//#############################################################################
// Callback-Funktion, die periodisch von X-Plane aufgerufen wird, wenn vom
// Plugin so gefordert...
// fuer Anzeige MFD benutzt
//#############################################################################
PLUGIN_API float myfLoopCallback( float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon)
{
  mainClass *pmCl;                                                   // Zeiger auf Main Klasse
  float res = float(1.0);                                            // Normaler Wert 1.0

  if( inRefcon )                                                     // Gibt es eine Refererenz...
  {
    pmCl = static_cast<mainClass *>(inRefcon);                       // Caste die Referenz auf den richtigen Typ
    res = pmCl->doMFDDisplay();                                      // Aktualisiere das Display
  }
  return( res );
}

//#############################################################################
// Callback-Funktion, die periodisch von X-Plane aufgerufen wird, wenn vom
// Plugin so gefordert...
// fuer Steuerung der LED's genutzt
//#############################################################################
PLUGIN_API float myfLEDLoopCallback( float inElapsedSinceLastCall, float inElapsedTimeSinceLastFlightLoop, int inCounter, void *inRefcon)
{
  mainClass *pmCl;                                                   // Zeiger auf Main Klasse
  float res = float(BLINKTIME);                                      // Normaler Wert

  if( inRefcon )                                                     // Gibt es eine Refererenz...
  {
    pmCl = static_cast<mainClass *>(inRefcon);                       // Caste die Referenz auf den richtigen Typ
    res = pmCl->doLEDStatus(inElapsedSinceLastCall);                 // Aktualisiere LED's
  }
  return( res );
}

//#############################################################################
// Startprozedur der X-Plane Schnittstelle. Wird beim Start des Simulators
// aufgerufen. Initialisiert das gesamte Plugin, erzeugt alle Objekte,
// startet alle Threads...
//#############################################################################
PLUGIN_API int XPluginStart( char *outName, char *outSig, char *outDesc )
{
  char outputPath[255];                                              // path for Login File 
  string logFileName;                                                // Das Ganze als String
  string configFileName;                                             // das auch noch
  //XPLMCommandRef cmdRef;                                             // Kommandoreferenz
  // Beschreibung und virtuellen Pfad des Plugins angeben
  strcpy_s( outName, 255, "SaitekX52Pro");
  strcpy_s( outSig, 255, "dmarc.saitek.x52Pro");
  strcpy_s( outDesc, 255, "Plugin for Display Flight Data on MFD.");
  XPLMGetSystemPath(outputPath);                                     // Wo bin ich?
  logFileName = string( outputPath ) + string( "Resources\\plugins\\saitekMFD.log" );
  configFileName = string( outputPath ) + string( "Resources\\plugins\\saitekMFD.conf" );

  try
  {
    pmCl = new mainClass( logFileName, configFileName );             // die hauptklasse die alles weitere macht
    pmCl->logLine( "<XPluginStart>: class <mainClass> created.");
    pmCl->logLine( "<XPluginStart>: pmCl->initProgrammData..." );
    if( ! pmCl->initProgrammData( ) )                                // Alle Daten, die ich noch brauche initialisieren
    {
      return(0);                                                     // DANEBEN!
    }
    pmCl->logLine( "<XPluginStart>: pmCl->initProgrammData...OK" );
    pmCl->logLine( "<XPluginStart>: Register FlightLoopCallback MFD (XPLMRegisterFlightLoopCallback)...");
    XPLMRegisterFlightLoopCallback(		                               // ich initialisiere einen Callback,
			                    myfLoopCallback,	                         // der über die Hilfsfunktioen myfLoopCallback
			                    float(1.0),	                               // Alle Sekunde aufgerufen wird
		                    	static_cast<void *>(pmCl) );	             // und mit der Referenz auf mainClass
    pmCl->logLine( "<XPluginStart>: Register FlightLoopCallback MFD (XPLMRegisterFlightLoopCallback)...OK");
    //
    pmCl->logLine( "<XPluginStart>: Register FlightLoopCallback LED (XPLMRegisterFlightLoopCallback)...");
    XPLMRegisterFlightLoopCallback(		                               // ich initialisiere einen Callback,
			                    myfLEDLoopCallback,	                       // der über die Hilfsfunktioen myfLEDLoopCallback
			                    float(0.4),		                             // Alle 0.4 Sekunde aufgerufen wird
		                    	static_cast<void *>(pmCl) );	             // und mit der Referenz auf mainClass
    pmCl->logLine( "<XPluginStart>: Register FlightLoopCallback LED (XPLMRegisterFlightLoopCallback)...OK");
    /*
    pmCl->logLine( "Create Commands (XPLMCreateCommand)...");
    // Pages UP Kommands
    cmdRef = XPLMCreateCommand("sim/instruments/X52ProDatapageUp", "cycles UP through datapages on the X52 display");
    pmCl->setMfdCycleUpCmdRef( cmdRef );                             // beim Mainobjekt hinterlegen
    XPLMRegisterCommandHandler( cmdRef, dispatchCommand, 0, (void *)pmCl ); // bei XPlane anmelden
    // Pages DOWN Kommando
    cmdRef = XPLMCreateCommand("sim/instruments/X52ProDatapageDown", "cycles DOWN through datapages on the X52 display");
    pmCl->setMfdCycleDownCmdRef( cmdRef );                           // beim Mainobjekt hinterlegen
    XPLMRegisterCommandHandler( cmdRef, dispatchCommand, 0, (void *)pmCl ); // bei XPlane anmelden
    // Scheint geklappt zu haben
    pmCl->logLine( "Create Commands (XPLMCreateCommand)...OK");
    */
  }
  catch( char * )
  {
    return(0);
  }
	return( 1 );
}

//#############################################################################
// Stop-Prozedur der X-Plane Schnittstelle. 
// sollte alle Thread anhalten und Datan freigeben
//#############################################################################
PLUGIN_API void	XPluginStop(void)
{
  XPLMUnregisterFlightLoopCallback( myfLoopCallback, NULL );
  if( pmCl ) 
  {
    pmCl->logLine( "XPluginStop recived!, Delete Main Object..." );
    delete pmCl;
    pmCl = 0;
  }
}

//#############################################################################
// Disabled das Plugin, Funktion für die Schnittstelle von X-Plane
//#############################################################################
PLUGIN_API void XPluginDisable(void)
{
  if( pmCl )
  {
    pmCl->setPluginEnabled( false );                                 // sag bescheid!
    pmCl->logLine( "XPluginDisable recived!" );
  }
}

//#############################################################################
// Plugin enable, wird von XPlane aufgerufen
//#############################################################################
PLUGIN_API int XPluginEnable(void)
{
  if( pmCl )
  {
    pmCl->setPluginEnabled( true );                                 // sag bescheid!
    pmCl->logLine( "XPluginEnable recived!" );
  }
	return 1;
}

//#############################################################################
// Nachrichtenschnittstelle von X-Plane, benutze ich hier nicht
//#############################################################################
PLUGIN_API void XPluginReceiveMessage( XPLMPluginID	inFromWho, long inMessage, void *	inParam )
{
  return;
}

//#############################################################################
// Empfängt Nachrichen über Kommandos von XPlane...
//#############################################################################
/*
PLUGIN_API int dispatchCommand( XPLMCommandRef cmd, XPLMCommandPhase phase, void *arg ) 
{
  if( pmCl )
  {
    return(pmCl->computeCommands( cmd, phase, arg ));                // Delegieren ist alles!
  }
	return 1;                                                          // Ich hab nix erledigt!
}
*/
