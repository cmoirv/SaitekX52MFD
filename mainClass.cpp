//#############################################################################
// Implementation der Hauptklasse des Projektes
//#############################################################################

#include "mainClass.hpp"
#include <sstream>

using namespace std;

// Als Call-Verteiler für eine Callbackfunktion hier lokal für dieses Modul deklarieren
void __stdcall softButtonCallBack( DWORD btns, void *ctxt );
void __stdcall pageChangeCallBack( DWORD pg, bool active, void *ctxt );
// Fürs Debugging Klarnamen machen
std::string __stdcall getStringForSelectionName( saSELECT );

// Statisch die sprintf Strings für die Anzeige auf dem MFD zurechtmachen
// ist so schneller...
wchar_t *displayFormat[14] = 
{
	L" %03d.%02d  %03d.%02d \0",                                        // Normal, nichts selektiert
	L"[%03d.%02d] %03d.%02d \0",                                        // COM X markiert zum Frequenztausch
	L">%03d.%02d  %03d.%02d \0",                                        // COM x Kiloherz markiert zum verstellen
	L" %03d.%02d< %03d.%02d \0",                                        // COM x Herz markuert zum verstellen
	L" %03d.%02d [%03d.%02d]\0",                                        // NAV X markiert zum Frequenztausch
	L" %03d.%02d >%03d.%02d \0",                                        // NAV X Kiloherz markiert zum verstellen
	L" %03d.%02d  %03d.%02d<\0",                                        // NAV X Herz markiert zum verstellen
	L" %03d.%02d  %03d.%02d \0",                                        // normal, nichts markiert
	L"[%03d.%02d] %03d.%02d \0",                                        // COM X markiert zum Frequenztausch
	L"*%03d.%02d  %03d.%02d \0",                                        // COM x Kiloherz selektiert zum verstellen
	L" %03d.%02d* %03d.%02d \0",                                        // COM x Herz selektiert zum verstellen
	L" %03d.%02d [%03d.%02d]\0",                                        // NAV X selektiert zum Frequenztausch
	L" %03d.%02d *%03d.%02d \0",                                        // NAV X Kiloherz selektiert zum verstellen
	L" %03d.%02d  %03d.%02d*\0",                                        // NAV X Herz selektiert zum verstellen
};

// pattern to display transponder
wchar_t *displayFormatTransponder[10] = 
{
	L"CODE : %d %d %d %d \0",
	L"CODE :[%d]%d %d %d \0",
	L"CODE : %d[%d]%d %d \0",
	L"CODE : %d %d[%d]%d \0",
	L"CODE : %d %d %d[%d]\0",
	L"CODE :>%d<%d %d %d \0",
	L"CODE : %d>%d<%d %d \0",
	L"CODE : %d %d>%d<%d \0",
	L"CODE : %d %d %d>%d<\0",
	L"                   \0"
};

// pattern to display transponder mode
wchar_t *displayFormatTransponderMode[3] = 
{
	L"MODE :  %s  \0",
	L"MODE : [%s] \0",
	L"MODE : >%s< \0"
};


//#############################################################################
// Standartkonstruktor (PRIVAT!)
//#############################################################################
mainClass::mainClass(void)
{
}

//#############################################################################
// Konstruktor der Klasse. Sorgt für Vorhandensein aller Strukturen, Objekte, 
// und Daten
//#############################################################################
mainClass::mainClass( string& logFileName, string& configFileName ) : 
debug(false),pPluginConfigClass(), plogThreadClass(0),groundSpeedRef(0),
	indicatedAirSpeedRef(0), verticalSpeedRef(0),elevationRev(0),
	com1Ref(0),com2Ref(0),com1StbyRef(0),com2StbyRef(0),
	nav1Ref(0),nav2Ref(0),nav1StbyRef(0),nav2StbyRef(0),
	currCursorSelect(saSEL_NONE),upDownMode(saUD_SELECT),clickMode(saCLICK_CHANGE),transponder(0),nav1(0),nav2(0)
{
	try
	{
		plogThreadClass = new logThreadClass( logFileName );             // Erzeuge den zentrale LOG Thread
		pPluginConfigClass = new pluginConfigClass( configFileName );    // Die Konfigurationsklasse erzeugen
		transponder= new Transponder();
		// instanciation of the 2 navs object
		nav1 = new Navs(NAV1);
		nav2 = new Navs(NAV2);

		// instanciation of autopilot object
		autopilot = new AutoPilot();

		pPluginConfigClass->setLogThreadClass( plogThreadClass );        // Klasse fuer Loggen bereitmachen
		pPluginConfigClass->readConfig();                                // Jetzt lese die Configuration!
		debug = pPluginConfigClass->isDebug();                           // Debugstatus eintragen
		plogThreadClass->setDebug( debug );                              // debuglevel setzen
		if( debug ) logLine( "<mainClass::mainClass> DEBUG ON..." );
		if( debug ) logLine( "<mainClass::mainClass> create Class saitekX52ProClass..." );
		psaitekX52ProClass = new saitekX52ProClass( plogThreadClass );   // erzeuge das Objekt für den Stick
		logLine( "<mainClass::mainClass> Init Stick..." ); 
		flushStream();                                                   // Sicherstellen, daß das i der Datei auch ankommt
		Sleep( 400 );                                                    // ich hoffe, das hilft ;-)
		if( debug ) logLine( "<mainClass::mainClass> call psaitekX52ProClass->stickFuncInit..." );
		psaitekX52ProClass->stickFuncInit( L"X-Plane MFD" );             // initialisiere das Teil, und gib der Instanz einen Namen
		if( debug ) logLine( "<mainClass::mainClass> call psaitekX52ProClass->registerSoftButtonCalback..." );
		psaitekX52ProClass->registerSoftButtonCalback( softButtonCallBack, this ); // für die Buttons am Display...
		if( debug ) logLine( "<mainClass::mainClass> call psaitekX52ProClass->registerPageChangeCallback..." );
		psaitekX52ProClass->registerPageChangeCallback( pageChangeCallBack, this ); // Seitenwechsel
		if( debug ) logLine( "<mainClass::mainClass> Init Stick...OK" );
		Sleep( 100 );                                                    // Zur Sicherheit etwas Zeit verstreichen lassen...
	}
	catch( mfdException& _ex )                                         // Sollte eine Ausnahmesituation auftreten...
	{
		errorLine( _ex.msg() );                                          // Die Meldung in die Logdatei
		flushStream();                                                   // Und gleich in die Daei durchschreiben
		throw;                                                           // Damit ist Schluß mit dem Plugin
	}
	catch( char * _msg )                                               // für unvorhergesehene Dinge 
	{
		// was tun Sprach Zeuss, die Götter sind besoffen...
		if( plogThreadClass )                                            // wenn das schon vorhanden war...
		{
			stringstream str;                                              // Das Formatiert sich viel besser
			string errorMsg = "Unerwarteter Fehler (";
			str << errorMsg << _msg << ") Zeile: " << __LINE__ << " in File <" << __FILE__ << ">";
			str >> errorMsg;
			errorLine( errorMsg );
			flushStream();                                                 // Ratz-Fatz wegschreiben, bevor es dunkel wird
		}
		throw;
	}
}

//#############################################################################
// der Destruktor
//#############################################################################
mainClass::~mainClass(void)
{
	if( pPluginConfigClass ) delete pPluginConfigClass;                // Config Klasse verschwinden lassen
	psaitekX52ProClass->registerSoftButtonCalback( 0, 0 );             // deregistrieren
	delete psaitekX52ProClass;                                         // Controller deinit
	if( plogThreadClass ) delete plogThreadClass;                      // Log Clas wegmachen
}

//#############################################################################
// Initialisiere alle für das Programm notwendigen Daten
//#############################################################################
bool mainClass::initProgrammData( void )
{
	int i;

	// besorge vom Simulator alle Referenzen, die ich später mal bauche
	try
	{
		if( debug ) logLine( "<mainClass::initProgrammData> Init Data..." );
		groundSpeedRef         = XPLMFindDataRef("sim/flightmodel/position/groundspeed");      // float
		indicatedAirSpeedRef   = XPLMFindDataRef("sim/flightmodel/position/indicated_airspeed2"); // float
		verticalSpeedRef       = XPLMFindDataRef("sim/flightmodel/position/vh_ind_fpm2");      // float
		//verticalSpeedRef       = XPLMFindDataRef("sim/flightmodel/position/vh_ind");           // float
		elevationRev           = XPLMFindDataRef("sim/flightmodel/position/elevation");        // float
		indicatedBaroElevRef   = XPLMFindDataRef("sim/flightmodel/misc/h_ind");                // float
		com1Ref                = XPLMFindDataRef("sim/cockpit/radios/com1_freq_hz");           //int
		com2Ref                = XPLMFindDataRef("sim/cockpit/radios/com2_freq_hz");           //int
		com1StbyRef            = XPLMFindDataRef("sim/cockpit/radios/com1_stdby_freq_hz");     //int
		com2StbyRef            = XPLMFindDataRef("sim/cockpit/radios/com2_stdby_freq_hz");     //int
		nav1Ref                = XPLMFindDataRef("sim/cockpit/radios/nav1_freq_hz");           //int
		nav2Ref                = XPLMFindDataRef("sim/cockpit/radios/nav2_freq_hz");           //int
		nav1StbyRef            = XPLMFindDataRef("sim/cockpit/radios/nav1_stdby_freq_hz");     //int
		nav2StbyRef            = XPLMFindDataRef("sim/cockpit/radios/nav2_stdby_freq_hz");     //int
		gearRetract            = XPLMFindDataRef("sim/aircraft/gear/acf_gear_retract");        //int (Fahrwerk in Aktion)
		gearDeploy             = XPLMFindDataRef("sim/aircraft/parts/acf_gear_deploy");        //float Fahrwerksposition
		gearIsSkid             = XPLMFindDataRef("sim/aircraft/gear/acf_gear_is_skid");        //int Fahrwerk ist FEST
		landingLightsOn        = XPLMFindDataRef("sim/cockpit/electrical/landing_lights_on");  //bool Landelichter AN?
		flapsDeploy            = XPLMFindDataRef("sim/cockpit2/controls/flap_handle_deploy_ratio"); // float Gesamt-Status Klappen
		simIsPaused            = XPLMFindDataRef("sim/time/paused");                           // int Sim ist in den ferien
		// Added by Cmoirv
		barometer_setting      = XPLMFindDataRef("sim/cockpit/misc/barometer_setting");
		//End added by Cmoirv
		for( i=0; i< MAXPAGES; i++ )
		{
			oldGearDeploy[i] = 5.0;
		}
		doLEDStatus(0.0);
		if( debug ) logLine( "<mainClass::initProgrammData> Init Data...OK" );
	}
	catch( char * _msg )                                               // für unvorhergesehene Dinge 
	{
		stringstream str;                                                // Das Formatiert sich viel besser
		string errorMsg = "<mainClass::initProgrammData> (";
		str << errorMsg << _msg << ") Zeile: " << __LINE__ << " in File <" << __FILE__ << ">";
		str >> errorMsg;
		errorLine( errorMsg );
		flushStream();
		return( false );
	}
	return( true );
}

//#############################################################################
// Das Display Aktualisieren! Wir oft aufgerufen!
//#############################################################################
float mainClass::doMFDDisplay( void )
{
	char wbuffer[36];                                                  // Buffer für sprintf (Ok, nicht elegant aber einfach ;-))
	int displayFormatIndex1 = 0;                                       // Format-Index für Zeile 1
	int displayFormatIndex2 = 0;                                       // Format Index für Zeile 2
	int displayTransponder = 0;
	int displayTransponderMode = 0;

	static int oldActivePage;                                          // Merk ich mir mal...
	int activePage = psaitekX52ProClass->getActivePage();              // was ist aktuell?

	if( oldActivePage != activePage )                                  // Gab es einen Seitenwechsel?
	{
		oldActivePage = activePage;                                      // Bei Seitenwechsel soll nix selektiert sein...
		currCursorSelect = saSEL_NONE;
		upDownMode = saUD_SELECT;//add cmoirv 
	}
	if ( ! psaitekX52ProClass ) return( 1.0 );                         // Kein Objekt, keine Aktion
	if( activePage > 0 )                                               // Auf Seite 0 brauch ich das alles nicht!
	{                                                                  // die anderen Seiten brauchen einen Index für die Anzeige
		switch( currCursorSelect )                                       // wo steht der Cursor?
		{
		case saSEL_NONE:                                                 // nichts selektiert
			displayFormatIndex1 = 0;
			displayFormatIndex2 = 0;
			if(transponder->getMode() == TRANSPONDER_OFF)						// manage transponder mode
			{
				displayTransponder = 9;
			}
			else
			{
				displayTransponder = 0;
			}
			displayTransponderMode = 0;
			break;
		case saSEL_TOPLEFT:                                              // Markierung oben links (COM1,COM2)
			displayFormatIndex1 = 1;
			displayFormatIndex2 = 0;
			break;
		case saSEL_BOTTOMLEFT_LEFT:                                      // Markierung Standby Frequenz COM X Kiloherz
			displayFormatIndex1 = 0;
			displayFormatIndex2 = 2;
			break;
		case saSEL_BOTTOMLEFT_RIGHT:                                     // Markierung Standby Frequenz COM X Herz
			displayFormatIndex1 = 0;
			displayFormatIndex2 = 3;
			break;
		case saSEL_TOPRIGHT:                                             // Markierung oben rechts, NAV X
			displayFormatIndex1 = 4;
			displayFormatIndex2 = 0;
			break;
		case saSEL_BOTTOMRIGHT_LEFT:                                     // Markierung Standby Frequenz NAV X Kiloherz
			displayFormatIndex1 = 0;
			displayFormatIndex2 = 5;
			break;
		case saSEL_BOTTOMRIGHT_RIGHT:                                    // Markierung Standby Frequenz NAV X Herz 
			displayFormatIndex1 = 0;
			displayFormatIndex2 = 6;
			break;
		case saSEL_TPR_THDS:											// transponder selection : thousands
			displayTransponder = 1;
			displayTransponderMode = 0;
			break;
		case saSEL_TPR_HDS: 											// transponder selection : hundreds                                   
			displayTransponder = 2;
			displayTransponderMode = 0;
			break;
		case saSEL_TPR_TENS:											// transponder selection : tens                                    
			displayTransponder = 3;
			displayTransponderMode = 0;
			break;
		case saSEL_TPR_UNTS:											// transponder selection : units                                   
			displayTransponder = 4;
			displayTransponderMode = 0;
			break;
		case saSEL_TPR_MODE:											// transponder selection : mode
			if(transponder->getMode() == TRANSPONDER_OFF)
			{
				displayTransponder = 9;
			}
			else
			{
				displayTransponder = 0;
			}

			displayTransponderMode = 1;
			break;
		}
	}
	if( upDownMode == saUD_VALUE )                                     // Markierung oder Selektion?
	{
		displayFormatIndex1 += 7;                                        // Selektionsmodus
		displayFormatIndex2 += 7;
		if(currCursorSelect == saSEL_TPR_MODE)
		{
			displayTransponderMode +=1;
		}
		else
		{
			displayTransponder +=4;
		}
	}
	// Display aktualisieren
	// Welche Seite ist aktiv?
	switch( activePage )
	{
	case ID_PAGE_SPEEDS:                                                            // Location - Seite (wie schnell bin ich, wie hoch)
		swprintf( (wchar_t *)&wbuffer[0], 16, L"HSPD:  %4d KIA\0", int( XPLMGetDataf( indicatedAirSpeedRef )));
		psaitekX52ProClass->setString( activePage, 0, wstring( (wchar_t *)&wbuffer[0] ) );
		swprintf( (wchar_t *)&wbuffer[0], 16, L"VVI :  %4d fts\0", int( XPLMGetDataf( verticalSpeedRef )));
		psaitekX52ProClass->setString( activePage, 1, wstring( (wchar_t *)&wbuffer[0] ) );
		swprintf( (wchar_t *)&wbuffer[0], 16, L"ELEV: %5d ft\0", int( XPLMGetDataf( elevationRev ) * 3.2808399 ));  //1 Meter = 3,2808399 Fuß
		psaitekX52ProClass->setString( activePage, 2, wstring( (wchar_t *)&wbuffer[0] ) );
		return( float(0.5) );                                            // in einer halbenSekunde wieder zurück!
		break;
	case ID_PAGE_COM1NAV1:                                                            // Seite 1, COM 1 und NAV 1
		psaitekX52ProClass->setString( activePage, 0, wstring( L"  COM1    NAV1  " ) );
		swprintf( (wchar_t *)&wbuffer[0], 17, displayFormat[displayFormatIndex1], 
			(XPLMGetDatai( com1Ref ) / 100),
			(XPLMGetDatai( com1Ref ) % 100),  
			(XPLMGetDatai( nav1Ref ) / 100),
			(XPLMGetDatai( nav1Ref ) % 100));  
		psaitekX52ProClass->setString( activePage, 1, wstring( (wchar_t *)&wbuffer[0] ) );
		swprintf( (wchar_t *)&wbuffer[0], 17, displayFormat[displayFormatIndex2], 
			(XPLMGetDatai( com1StbyRef ) / 100),
			(XPLMGetDatai( com1StbyRef ) % 100),  
			(XPLMGetDatai( nav1StbyRef ) / 100),
			(XPLMGetDatai( nav1StbyRef ) % 100)); 
		psaitekX52ProClass->setString( activePage, 2, wstring( (wchar_t *)&wbuffer[0] ) );
		return( float(1.5) );                                            // erst in eineinhalb Sekunden zurück
		break;
	case ID_PAGE_COM2NAV2:                                                            // Seite 2, COM 2 und NAV 2
		psaitekX52ProClass->setString( activePage, 0, wstring( L"  COM2    NAV2  " ) );
		swprintf( (wchar_t *)&wbuffer[0], 17, displayFormat[displayFormatIndex1], 
			(XPLMGetDatai( com2Ref ) / 100),
			(XPLMGetDatai( com2Ref ) % 100),  
			(XPLMGetDatai( nav2Ref ) / 100),
			(XPLMGetDatai( nav2Ref ) % 100));
		psaitekX52ProClass->setString( activePage, 1, wstring( (wchar_t *)&wbuffer[0] ) );
		swprintf( (wchar_t *)&wbuffer[0], 17, displayFormat[displayFormatIndex2], 
			(XPLMGetDatai( com2StbyRef ) / 100),
			(XPLMGetDatai( com2StbyRef ) % 100),  
			(XPLMGetDatai( nav2StbyRef ) / 100),
			(XPLMGetDatai( nav2StbyRef ) % 100));
		psaitekX52ProClass->setString( activePage, 2, wstring( (wchar_t *)&wbuffer[0] ) );
		return( float(1.5) );                                            // in eineinhalb Sekunden zurück
		break;

	case ID_PAGE_QNH:            // Added selection of QNH by Cmoirv                                           
		psaitekX52ProClass->setString( activePage, 0, wstring( L" Baro Setting  \0" ) );
		swprintf( (wchar_t *)&wbuffer[0], 16, L" %02.2f mmHg \0", XPLMGetDataf(barometer_setting) );
		psaitekX52ProClass->setString( activePage, 1, wstring( (wchar_t *)&wbuffer[0] ) );
		swprintf( (wchar_t *)&wbuffer[0], 16, L" %04.0f hPa \0", XPLMGetDataf(barometer_setting) * 254 * 0.133322368421);
		psaitekX52ProClass->setString( activePage, 2, wstring( (wchar_t *)&wbuffer[0] ) );

		return( float(0.5) );                                            
		break;// 
	case ID_PAGE_TRANSPONDER:           // Added selection of squawk code and transponder mode by Cmoirv   
		psaitekX52ProClass->setString( activePage, 0, wstring( L" Transponder  \0" ) );
		swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatTransponder[displayTransponder], 
			transponder->getThousands(), 
			transponder->getHundreds(), 
			transponder->getTens(), 
			transponder->getUnits());
		psaitekX52ProClass->setString( activePage, 1, wstring( (wchar_t *)&wbuffer[0] ) );
		swprintf( (wchar_t *)&wbuffer[0], 16, displayFormatTransponderMode[displayTransponderMode], 
			transponder->getModeString());
		psaitekX52ProClass->setString( activePage, 2, wstring( (wchar_t *)&wbuffer[0] ) );

		return( float(0.5) );                                           
		break;
	case ID_PAGE_NAV1:           // Added obs selection for nav 1 by Cmoirv   
		nav1->doMFDDisplay(psaitekX52ProClass);

		return( float(0.5) );                                           
		break;

	case ID_PAGE_NAV2:           // Added obs selection for nav 2 by Cmoirv   
		nav2->doMFDDisplay(psaitekX52ProClass);

		return( float(0.5) );                                           
		break;

	case ID_PAGE_AUTOPILOT:           // Added obs selection for nav 2 by Cmoirv   
		autopilot->doMFDDisplay(psaitekX52ProClass);

		return( float(0.5) );                                           
		break;
	default:
		logLine( "<mainClass::doMFDDisplay>: Invalid MFD-Page!" );
		return( float(3.0) );
		break;
	}
	return( float(0.6) );
}

//#############################################################################
// Das Display Aktualisieren! Wir oft aufgerufen!
//#############################################################################
float mainClass::doLEDStatus( float inElapsedSinceLastCall )
{
	bool wasBlinking=false;
	float nextCall = float(NOBLINKTIME);
	bool alwaysSet = false;
	int activePage = psaitekX52ProClass->getActivePage();              // was ist aktuell?
	float locGearDeploy[10];                                           // Wie stehen die Dinge beim Fahrwerk
	saLED led;                                                         // um welche LED handelt es sich?

	if( inElapsedSinceLastCall == 0.0 )
	{
		alwaysSet = true;
	}

	// landing gear? 
	XPLMGetDatavf( gearDeploy, locGearDeploy, 0, 10 );                 // Alle 10 Fahrwerke kopiern
	led = pPluginConfigClass->getlandingGearLed();                     // wer könnte es sein?
	if( (led != NONE_LED) && (0 == XPLMGetDatai(gearIsSkid)) )         // wenn LED vorhanden und KEIN festes Fahrwerk
		processGearLed( activePage, led );                               // Fahrwerks-LED steuern
	// flaps? 
	led = pPluginConfigClass->getflapsGearLed();
	if( led != NONE_LED )                                               // wenn LED vorhanden
		processFlapsLed( activePage, led );                               // GEAR-LED steuern
	// running/pausing LED?
	led = pPluginConfigClass->getIsRunningLED();
	if( led != NONE_LED )                                               // wenn LED vorhanden
		processIsRunningLed( activePage, led );                           // Running-LED steuern

	if( debug ) 
	{
		char buffer[255]; 
		//logLine( "<mainClass::doLEDStatus> ..." );
		sprintf_s( buffer, 255, "<mainClass::doLEDStatus> timeSinceLastCall: %f", inElapsedSinceLastCall );
		logLine( buffer );
	}

	if( psaitekX52ProClass->makeAllLedBlink( alwaysSet )) wasBlinking = true;
	if( wasBlinking ) nextCall = float(BLINKTIME);                     // Wenn es blinken soll, dann demnächst wieder

	//if( debug ) logLine( "<mainClass::doLEDStatus>...OK" );
	return( nextCall );
}

//#############################################################################
// Den ausgewählten Wert hoch- oder runterzählen
//#############################################################################
void mainClass::countValue( saDIRECTION dir )
{
	int diff;

	int old_freq = 0;

	if( dir == saCOUNT_UP )                                            // Das Vorzeichen gibt natürlich die Richtung vor
	{
		diff = 1;
		if( debug ) logLine( "<mainClass::countValue> (count Value <UP> - MFD right weel)" );
	}
	else if( dir == saCOUNT_DOWN )
	{
		diff = -1;
		if( debug ) logLine( "<mainClass::countValue> (count Value <DOWN> - MFD right weel)" );
	}
	// zunächst bekommen mal raus, WAS ich hoch/runter ackern soll
	if( currCursorSelect == saSEL_BOTTOMLEFT_LEFT )                    // entweder COM1 oder COM2 Standby Kiloherz
	{
		if( debug ) logLine( "<mainClass::countValue> Value bottom left-left value..." );
		diff *= 100;                                                     // Kiloherz stellen
		if( ID_PAGE_COM1NAV1 == psaitekX52ProClass->getActivePage() )                   // COM1 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...COM1 standby, kHz!" );
			XPLMSetDatai( com1StbyRef, XPLMGetDatai(com1StbyRef) + diff  );// Stellen

			if(XPLMGetDatai(com1StbyRef) > 13697) // reach the higher limit
				XPLMSetDatai( com1StbyRef, XPLMGetDatai(com1StbyRef) - 1900);
			else if(XPLMGetDatai(com1StbyRef) < 11800) // reach the lower limit
				XPLMSetDatai( com1StbyRef, XPLMGetDatai(com1StbyRef) + 1900);
		}
		else if( ID_PAGE_COM2NAV2 == psaitekX52ProClass->getActivePage() )              // COM2 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...COM2 standby, kHz!" );
			XPLMSetDatai( com2StbyRef, XPLMGetDatai(com2StbyRef) + diff  );// Stellen

			if(XPLMGetDatai(com2StbyRef) > 13697) // reach the higher limit
				XPLMSetDatai( com2StbyRef, XPLMGetDatai(com2StbyRef) - 1900);
			else if(XPLMGetDatai(com2StbyRef) < 11800) // reach the lower limit
				XPLMSetDatai( com2StbyRef, XPLMGetDatai(com2StbyRef) + 1900);
		}
	}

	else if( currCursorSelect == saSEL_BOTTOMLEFT_RIGHT )              // entweder COM1 oder COM2 Standby Herz
	{
		if( debug ) logLine( "<mainClass::countValue> Value bottom left-right value..." );
		if( ID_PAGE_COM1NAV1 == psaitekX52ProClass->getActivePage() )                   // COM1 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...COM1 standby, Hz!" );

			old_freq = XPLMGetDatai(com1StbyRef);
			XPLMSetDatai( com1StbyRef, XPLMGetDatai(com1StbyRef) + diff  );// Stellen

			if((XPLMGetDatai(com1StbyRef)/100)-(old_freq/100) == 1 || (XPLMGetDatai(com1StbyRef)/100)-(old_freq/100) == -1){
				XPLMSetDatai( com1StbyRef, XPLMGetDatai(com1StbyRef) - (diff) * 100);
			}
		}
		else if( ID_PAGE_COM2NAV2 == psaitekX52ProClass->getActivePage() )              // COM2 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...COM2 standby, Hz!" );

			old_freq = XPLMGetDatai(com2StbyRef);
			XPLMSetDatai( com2StbyRef, XPLMGetDatai(com2StbyRef) + diff  );// Stellen

			if((XPLMGetDatai(com2StbyRef)/100)-(old_freq/100) == 1 || (XPLMGetDatai(com2StbyRef)/100)-(old_freq/100) == -1){
				XPLMSetDatai( com2StbyRef, XPLMGetDatai(com2StbyRef) - (diff) * 100);
			}
		}
	}

	else if( currCursorSelect == saSEL_BOTTOMRIGHT_LEFT )              // entweder NAV1 oder NAV2 Standby Kiloherz
	{
		if( debug ) logLine( "<mainClass::countValue> Value bottom right left value..." );
		diff *= 100;                                                     // Kiloherz stellen
		if( ID_PAGE_COM1NAV1 == psaitekX52ProClass->getActivePage() )                   // NAV1 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...NAV1 standby, kHz!" );
			XPLMSetDatai( nav1StbyRef, XPLMGetDatai(nav1StbyRef) + diff  );// Stellen

			// Added restriction of the NAV frequencies by Cmoirv (to be closer to the reality)
			if(XPLMGetDatai(nav1StbyRef) > 11795) // reach the higher limit
				XPLMSetDatai( nav1StbyRef, XPLMGetDatai(nav1StbyRef) - 1000);
			else if(XPLMGetDatai(nav1StbyRef) < 10800) // reach the lower limit
				XPLMSetDatai( nav1StbyRef, XPLMGetDatai(nav1StbyRef) + 1000);
		}
		else if( ID_PAGE_COM2NAV2 == psaitekX52ProClass->getActivePage() )              // NAV2 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...NAV2 standby, kHz!" );
			XPLMSetDatai( nav2StbyRef, XPLMGetDatai(nav2StbyRef) + diff  );// Stellen

			// Added restriction of the NAV frequencies by Cmoirv (to be closer to the reality)
			if(XPLMGetDatai(nav2StbyRef) > 11795) // reach the higher limit
				XPLMSetDatai( nav2StbyRef, XPLMGetDatai(nav2StbyRef) - 1000);
			else if(XPLMGetDatai(nav2StbyRef) < 10800) // reach the lower limit
				XPLMSetDatai( nav2StbyRef, XPLMGetDatai(nav2StbyRef) + 1000);
		}
	}

	else if( currCursorSelect == saSEL_BOTTOMRIGHT_RIGHT )             // entweder NAV1 oder NAV2 Standby Herz
	{
		diff*=5; // Added by Cmoirv : 50 kHz spacing
		if( debug ) logLine( "<mainClass::countValue> Value bottom right right value..." );
		if( ID_PAGE_COM1NAV1 == psaitekX52ProClass->getActivePage() )                   // NAV1 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...NAV1 standby, Hz!" );

			old_freq = XPLMGetDatai(nav1StbyRef);
			XPLMSetDatai( nav1StbyRef, XPLMGetDatai(nav1StbyRef) + diff  );// Stellen

			// Added restriction of the NAV frequencies by Cmoirv (to be closer to the reality)
			if((XPLMGetDatai(nav1StbyRef)/100)-(old_freq/100) == 1 || (XPLMGetDatai(nav1StbyRef)/100)-(old_freq/100) == -1){
				XPLMSetDatai( nav1StbyRef, XPLMGetDatai(nav1StbyRef) - (diff/5) * 100);
			}

		}
		else if( ID_PAGE_COM2NAV2 == psaitekX52ProClass->getActivePage() )              // NAV2 Standby
		{
			if( debug ) logLine( "<mainClass::countValue> ...NAV2 standby, Hz!" );

			old_freq = XPLMGetDatai(nav2StbyRef);
			XPLMSetDatai( nav2StbyRef, XPLMGetDatai(nav2StbyRef) + diff  );// Stellen

			// Added restriction of the NAV frequencies by Cmoirv (to be closer to the reality)
			if((XPLMGetDatai(nav2StbyRef)/100)-(old_freq/100) == 1 || (XPLMGetDatai(nav2StbyRef)/100)-(old_freq/100) == -1){
				XPLMSetDatai( nav2StbyRef, XPLMGetDatai(nav2StbyRef) - (diff/5) * 100);
			}
		}
	}// Added transponder selection by Cmoirv 
	else if( currCursorSelect == saSEL_TPR_THDS ) 
	{
		transponder->setThousands(transponder->getThousands() + diff);
	}
	else if( currCursorSelect == saSEL_TPR_HDS ) 
	{
		transponder->setHundreds(transponder->getHundreds() + diff);
	}
	else if( currCursorSelect == saSEL_TPR_TENS ) 
	{
		transponder->setTens(transponder->getTens() + diff);
	}
	else if( currCursorSelect == saSEL_TPR_UNTS ) 
	{
		transponder->setUnits(transponder->getUnits() + diff);
	}
	else if( currCursorSelect == saSEL_TPR_MODE ) 
	{
		transponder->setMode(transponder->getMode() + diff);
	}
	else if( currCursorSelect == saSEL_NONE ) 
	{
		if( ID_PAGE_NAV1 == psaitekX52ProClass->getActivePage() )     // NAV1 
		{
			nav1->countValue(dir);
		}
		else if( ID_PAGE_NAV2 == psaitekX52ProClass->getActivePage()) // NAV2 
		{
			nav2->countValue(dir);
		}  
	}
}

//#############################################################################
// Tausche Frequenzen Standby <-> aktiv
//#############################################################################
void mainClass::changeFrequence( void )
{
	int standby;

	if( debug ) logLine( "<changeFrequence> (change frequence - reserve <-> active - MFD right weel)" );
	// erst mal rauskriegen, wo getauscht werden soll
	if( currCursorSelect == saSEL_TOPLEFT )                            // COM1 oder COM2
	{
		if( ID_PAGE_COM1NAV1 == psaitekX52ProClass->getActivePage() )                   // COM1
		{
			if( debug ) logLine( "<changeFrequence> COM1" );
			standby = XPLMGetDatai(com1StbyRef);                           // Standby sichern
			XPLMSetDatai( com1StbyRef, XPLMGetDatai(com1Ref) );            // aktiv auf standby
			XPLMSetDatai( com1Ref, standby );                              // standby auf aktiv
		}
		else if( ID_PAGE_COM2NAV2 == psaitekX52ProClass->getActivePage() )              // COM2
		{
			if( debug ) logLine( "<changeFrequence> COM2" );
			standby = XPLMGetDatai(com2StbyRef);                           // Standby sichern
			XPLMSetDatai( com2StbyRef, XPLMGetDatai(com2Ref) );            // aktiv auf standby
			XPLMSetDatai( com2Ref, standby );                              // standby auf aktiv
		}
	}
	else if( currCursorSelect == saSEL_TOPRIGHT )                      // NAV1 oder NAV2
	{
		if( ID_PAGE_COM1NAV1 == psaitekX52ProClass->getActivePage() )                   // nav1
		{
			if( debug ) logLine( "<changeFrequence> NAV1" );
			standby = XPLMGetDatai(nav1StbyRef);                           // Standby sichern
			XPLMSetDatai( nav1StbyRef, XPLMGetDatai(nav1Ref) );            // aktiv auf standby
			XPLMSetDatai( nav1Ref, standby );                              // standby auf aktiv
		}
		else if( ID_PAGE_COM2NAV2 == psaitekX52ProClass->getActivePage() )              // nav2
		{
			if( debug ) logLine( "<changeFrequence> NAV2" );
			standby = XPLMGetDatai(nav2StbyRef);                           // Standby sichern
			XPLMSetDatai( nav2StbyRef, XPLMGetDatai(nav2Ref) );            // aktiv auf standby
			XPLMSetDatai( nav2Ref, standby );                              // standby auf aktiv
		}
	}
}


//#############################################################################
// Auswahl auf der Seite ändern
//#############################################################################
void mainClass::cycleSelection( saDIRECTION dir )
{
	int activePage = psaitekX52ProClass->getActivePage(); 

	if( debug ) logLine( "<mainClass::cycleSelection>..." );
	// nur bei active page == 0 
	//##########################################################
	if( activePage == 0 )
	{
		if( debug ) 
		{
			logLine( "<mainClass::cycleSelection> Page 0 => No Action here ;-))" );
		}
		return;
	}
	//##########################################################
	switch(activePage){
	case ID_PAGE_COM1NAV1:
	case ID_PAGE_COM2NAV2:
		switch( currCursorSelect )                                         // WAS ist denn schon selektiert?
		{
		case saSEL_NONE:                                                   // nix selektiert
			if( dir == saCOUNT_UP )                                          // nach oben (raufzählen?)
				currCursorSelect = saSEL_BOTTOMRIGHT_RIGHT;
			else                                                             // oder nach unten
				currCursorSelect = saSEL_TOPLEFT;
			break;
		case saSEL_TOPLEFT:                                                // IST: COM X Wechseln
			if( dir == saCOUNT_UP )                                          // hoch oder runter?
				currCursorSelect = saSEL_NONE;
			else
				currCursorSelect = saSEL_BOTTOMLEFT_LEFT;
			break;
		case saSEL_BOTTOMLEFT_LEFT:                                        // IST: Standby COM x Kiloherz
			if( dir == saCOUNT_UP )
				currCursorSelect = saSEL_TOPLEFT;
			else
				currCursorSelect = saSEL_BOTTOMLEFT_RIGHT;
			break;
		case saSEL_BOTTOMLEFT_RIGHT:                                       // IST: COM X Herz
			if( dir == saCOUNT_UP )
				currCursorSelect = saSEL_BOTTOMLEFT_LEFT;
			else
				currCursorSelect = saSEL_TOPRIGHT;
			break;
		case saSEL_TOPRIGHT:                                               // IST: NAV X wechseln
			if( dir == saCOUNT_UP )
				currCursorSelect = saSEL_BOTTOMLEFT_RIGHT;
			else
				currCursorSelect = saSEL_BOTTOMRIGHT_LEFT;
			break;
		case saSEL_BOTTOMRIGHT_LEFT:                                       // IST: NAV X Kiloherz
			if( dir == saCOUNT_UP )
				currCursorSelect = saSEL_TOPRIGHT;
			else
				currCursorSelect = saSEL_BOTTOMRIGHT_RIGHT;
			break;
		case saSEL_BOTTOMRIGHT_RIGHT:                                     // IST: NAV X Herz
			if( dir == saCOUNT_UP )
				currCursorSelect = saSEL_BOTTOMRIGHT_LEFT;
			else
				currCursorSelect = saSEL_NONE;
			break;
		}
		if( debug ) logLine( "<mainClass::cycleSelection>: " + getStringForSelectionName( currCursorSelect ) );
		break;
	case ID_PAGE_QNH: // Added count for QNH by Cmoirv
		if( dir == saCOUNT_UP )
		{
			XPLMSetDataf(barometer_setting, XPLMGetDataf(barometer_setting) + 0.01f);
			if(XPLMGetDataf(barometer_setting) > 31)
			{
				XPLMSetDataf(barometer_setting, 31.0f);
			}
		}
		else
		{
			XPLMSetDataf(barometer_setting, XPLMGetDataf(barometer_setting) - 0.01f);
			if(XPLMGetDataf(barometer_setting) < 28)
			{
				XPLMSetDataf(barometer_setting, 28.0f);
			}
		}
		break;
	case ID_PAGE_TRANSPONDER: // Added transponder selection by Cmoirv
		switch( currCursorSelect )                                        
		{
		case saSEL_NONE:
		default: // in case of currCursorSelect is not in the value for transponder
			if( dir == saCOUNT_UP )                                          
				currCursorSelect = saSEL_TPR_MODE;
			else
			{
				if(transponder->getMode() != TRANSPONDER_OFF)
				{
					currCursorSelect = saSEL_TPR_THDS;
				}
				else
				{
					currCursorSelect = saSEL_TPR_MODE;
				}	  
			}

			break;
		case saSEL_TPR_THDS:                                                   
			if( dir == saCOUNT_UP )                                          
				currCursorSelect = saSEL_NONE;
			else                                                            
				currCursorSelect = saSEL_TPR_HDS;
			break;
		case saSEL_TPR_HDS:                                                   
			if( dir == saCOUNT_UP )                                          
				currCursorSelect = saSEL_TPR_THDS;
			else                                                             
				currCursorSelect = saSEL_TPR_TENS;
			break;
		case saSEL_TPR_TENS:                                                   
			if( dir == saCOUNT_UP )                                          
				currCursorSelect = saSEL_TPR_HDS;
			else                                                             
				currCursorSelect = saSEL_TPR_UNTS;
			break;
		case saSEL_TPR_UNTS:                                                   
			if( dir == saCOUNT_UP )                                          
				currCursorSelect = saSEL_TPR_TENS;
			else                                                             
				currCursorSelect = saSEL_TPR_MODE;
			break;
		case saSEL_TPR_MODE:    
			if(transponder->getMode() != TRANSPONDER_OFF)
			{
				if( dir == saCOUNT_UP )                                          
					currCursorSelect = saSEL_TPR_UNTS;
				else                                                             
					currCursorSelect = saSEL_NONE;
			}
			else
				currCursorSelect = saSEL_NONE;
			break;
		}
	case ID_PAGE_NAV1:
		nav1->countValue(dir);
		break;

	case ID_PAGE_NAV2:
		nav2->countValue(dir);
		break;
	}
	doMFDDisplay();                                                    // sofort anzeigen!
}

//#############################################################################
// Commands von X-Plane ausführen...
//#############################################################################
/*
int mainClass::computeCommands( XPLMCommandRef cmd, XPLMCommandPhase phase, void *arg )
{
if( phase == xplm_CommandBegin )                                   // Am Beginn des Kommandos
{
errorLine( "<mainClass::computeCommands> unknown Command from X-Plane recived" );
return(0);                                                       // ich habs erledigt!
}
return(1);                                                         // nix gemacht, laß noch andere dran!
}
*/

//#############################################################################
// Wird ein Softbutton betätigt, passiert hoffentlich das hier...
//#############################################################################
void mainClass::softButton( DWORD btns )
{
	//
	// Jetzt ist entscheidend, wo sich der Cursor befindet
	//
	switch( currCursorSelect )                                         // Wo steht die Selektion?
	{
	case saSEL_NONE:                                                   // Ist grade nix Selektiert
		if( btns & SoftButton_Up )
			cycleSelection( saCOUNT_UP );                                  // Selektion hochzählen
		else if( btns & SoftButton_Down )
			cycleSelection( saCOUNT_DOWN );                                // Selektion runterzählen
		break;

	case saSEL_TOPLEFT:                                                // Links oben (COM1 oder COM2  selektiert)
	case saSEL_TOPRIGHT:                                               // Rechts oben (NAV1 oder NAV2 selektiert
		if( btns & SoftButton_Select )                                   // wenn SELECT gedrückt
			changeFrequence();                                             // Aktive und Standby Frequenz tauschen
		else if( btns & SoftButton_Up )                                  // aufwärts gewählt
			cycleSelection( saCOUNT_UP );                                  // hier IMMER Selektion wechseln
		else if( btns & SoftButton_Down )                                // abwärts gewählt
			cycleSelection( saCOUNT_DOWN );                                 // hier IMMEr Selektion wechseln
		break;

	case saSEL_BOTTOMLEFT_LEFT:                                        // Kiloherz COM Standby ändern 
	case saSEL_BOTTOMLEFT_RIGHT:                                       // Herz COM Standby ändern
	case saSEL_BOTTOMRIGHT_LEFT:                                       // Kiloherz NAV Standby ändern
	case saSEL_BOTTOMRIGHT_RIGHT:                                      // Herz NAV Standby ändern
	case saSEL_TPR_THDS: 
	case saSEL_TPR_HDS:                                      
	case saSEL_TPR_TENS:                                       
	case saSEL_TPR_UNTS:
	case saSEL_TPR_MODE:
		if( btns & SoftButton_Select )                                   // Wechsel der Betriebsart (Ändern - zum Ändern wählen)
			if( upDownMode == saUD_SELECT )                                // Bin ich noch im Select-Mode?
				upDownMode = saUD_VALUE;                                     // dann mal in den Änderungsmodus
			else
				upDownMode = saUD_SELECT;                                    // oder eben den Select-Mode wählen
		else if( btns & SoftButton_Up )                                  // Aufwärts-Aktion
			if( upDownMode == saUD_SELECT )                                // bin ich noch im Select Mode
				cycleSelection( saCOUNT_UP );                                // dann ändere ich hier die Selektion
			else
				countValue( saCOUNT_UP );                                    // Wert ändern
		else if( btns & SoftButton_Down )
			if( upDownMode == saUD_SELECT )                                // bin ich noch im Select Mode
				cycleSelection( saCOUNT_DOWN );                              // dann ändere ich hier die Selektion
			else
				countValue( saCOUNT_DOWN );                                  // Wert ändern
		break;
	}
	doMFDDisplay();                                                    // Anzeige aktualisieren
}

//#############################################################################
// Bei Wechsel der Seite aufzurufen
//#############################################################################
void mainClass::pageChange( DWORD page, bool active )
{
	char buffer[255];

	if( debug )
	{
		sprintf_s( &buffer[0], 255, "<mainClass::pageChange>: PAGE CHANGE, Page %d, %s\0", int( 0xffffffff & page ), (active ? "true" : "false") );
		logLine( buffer );
	}
	doMFDDisplay();
	doLEDStatus( 0.0 );
}

//#############################################################################
// Daten ins Logfile kitten, dabei formatieren
//#############################################################################
bool mainClass::logLine( std::string& line )
{
	if( plogThreadClass )
		return( plogThreadClass->logLine( line ) );
	return(false);
}

//#############################################################################
// Daten ins Logfile kitten, dabei formatieren (auch mit char* )
//#############################################################################
bool mainClass::logLine( char *line )
{
	if( plogThreadClass )
		return( plogThreadClass->logLine( line ) );
	return(false);
}

//#############################################################################
// Fehlerausgabe mit *char
//#############################################################################
bool mainClass::errorLine( char *line )
{
	if( plogThreadClass )
		return( plogThreadClass->errorLine( line ) );
	return(false);
}

//#############################################################################
// Fehlerausgabe mit string
//#############################################################################
bool mainClass::errorLine( std::string& line )
{
	if( plogThreadClass )
		return( plogThreadClass->errorLine( line ) );
	return(false);
}

//#############################################################################
// Outstream flushen!
//#############################################################################
void mainClass::flushStream(void)
{
	if( plogThreadClass )
		return( plogThreadClass->flushStream() );
}

//#############################################################################
// entscheide über die Farbe der running/pause LED 
//#############################################################################
bool mainClass::processIsRunningLed( int page, saLED led )
{
	int locIsPaused = XPLMGetDatai( simIsPaused );                         // Ferien oder nicht?
	saLedStat pauseStat;

	if( locIsPaused == 1 )                                             // Ferien
	{
		pauseStat = pPluginConfigClass->getIsRunningColorForStat(SA_OK2);
		psaitekX52ProClass->setLed( page, led, pauseStat.first, pauseStat.second );
		return( true );
	}
	else
	{
		pauseStat = pPluginConfigClass->getIsRunningColorForStat(SA_OK1);
		psaitekX52ProClass->setLed( page, led, pauseStat.first, pauseStat.second );
		return( true );
	}
}

//#############################################################################
// entscheide ueber die Farbe der GEAR-LED
//#############################################################################
bool mainClass::processGearLed( int page, saLED led )
{
	float locGearDeploy[10];                                           // Wie stehen die Dinge beim Fahrwerk
	saLedStat gearStat;

	XPLMGetDatavf( gearDeploy, locGearDeploy, 0, 10 );                 // Alle 10 Fahrwerke kopiern

	if( oldGearDeploy[0] != locGearDeploy[0] )                         // hat sich was verändert?
	{
		if( locGearDeploy[0] == 1.0 )                                   // Fahrwerk ist UNTEN/DOWN
		{
			gearStat = pPluginConfigClass->getGearColorForStat(SA_OK2);
			psaitekX52ProClass->setLed( page, led, gearStat.first, gearStat.second );
			oldGearDeploy[0] = 1.0;                                       // Wert übernehmen
			return( true );
		}
		if( locGearDeploy[0] == 0.0 )                                   // Fahrwerk ist Oben/UP
		{
			gearStat = pPluginConfigClass->getGearColorForStat(SA_OK1);
			psaitekX52ProClass->setLed( page, led, gearStat.first, gearStat.second );
			oldGearDeploy[0] = 0.0;                                       // Wert übernehmen
			return( true );
		}
		if( locGearDeploy[0] > 0.0 && locGearDeploy[0] < 1.0 )          // Transfer...
		{
			if( oldGearDeploy[0] != 0.5 )                                 // nur ein mal
			{
				gearStat = pPluginConfigClass->getGearColorForStat(SA_TRANS);
				psaitekX52ProClass->setLed( page, led, gearStat.first, gearStat.second );
				oldGearDeploy[0] = 0.5;                                    // Wert übernehmen
			}
			return( true );
		}
		if( locGearDeploy[0] > 1.0 )
		{
			gearStat = pPluginConfigClass->getGearColorForStat(SA_FAILED);
			psaitekX52ProClass->setLed( page, led, gearStat.first, gearStat.second );
			oldGearDeploy[0] = locGearDeploy[0];                         // Wert übernehmen
			errorLine( "<mainClass::processGearLed> gearDeploy > 1.0 ==> Gear damaged?");
		}
	}
	return( true );
}

//#############################################################################
// entscheide ueber die Farbe der FLAPS-LED
//#############################################################################
bool mainClass::processFlapsLed( int page, saLED led )
{
	float locFlapsDeploy;                                              // Wie stehen die Dinge beim Fahrwerk
	saLedStat flapStat;

	locFlapsDeploy = XPLMGetDataf( flapsDeploy );                      // Wie gehts den Klappen?

	if( oldFlapsDeploy != locFlapsDeploy )                             // hat sich was verändert?
	{
		if( locFlapsDeploy == 0.0 )                                     // Klappen eingefahren
		{
			flapStat = pPluginConfigClass->getFlapsColorForStat(SA_OK1);
			psaitekX52ProClass->setLed( page, led, flapStat.first, flapStat.second );
			oldFlapsDeploy = locFlapsDeploy;                              // Wert übernehmen
			return( true );
		}
		if( locFlapsDeploy == 1.0 )                                     // Klappen VOLL / FULL
		{
			flapStat = pPluginConfigClass->getFlapsColorForStat(SA_OK2);
			psaitekX52ProClass->setLed( page, led, flapStat.first, flapStat.second );
			oldFlapsDeploy = locFlapsDeploy;                              // Wert übernehmen
			return( true );
		}
		if( locFlapsDeploy > 0.0 && locFlapsDeploy < 1.0 )              // Transfer/Zwischenstellung...
		{
			if( oldFlapsDeploy != 0.5 )                                   // nur ein mal
			{
				flapStat = pPluginConfigClass->getFlapsColorForStat(SA_TRANS);
				psaitekX52ProClass->setLed( page, led, flapStat.first, flapStat.second );
				oldFlapsDeploy = 0.5;                                       // Wert übernehmen
			}
			return( true );
		}
		if( locFlapsDeploy > 1.0 )
		{
			flapStat = pPluginConfigClass->getFlapsColorForStat(SA_FAILED);
			psaitekX52ProClass->setLed( page, led, flapStat.first, flapStat.second );
			oldFlapsDeploy = locFlapsDeploy;                              // Wert übernehmen
			errorLine( "<mainClass::processFlapsLed> flapsDeploy > 1.0 ==> Flaps damaged?");
		}
	}
	return( true );
}

//#############################################################################
//#############################################################################
// Lokale Hilfsroutinen
//#############################################################################
//#############################################################################


//#############################################################################
// Externe Funktion um das Objekt anmzusprechen...
//#############################################################################
void __stdcall softButtonCallBack( DWORD btns, void *ctxt )
{
	mainClass *classPtr;

	if( ctxt )
	{
		classPtr = static_cast<mainClass *>(ctxt);                       // Zeiger auf Objekt Casten...
		classPtr->softButton( btns );                                    // an den richtigen Member weiterleiten
	}
}

//#############################################################################
// Externe Funktion um das Objekt anmzusprechen...
//#############################################################################
void __stdcall pageChangeCallBack( DWORD page, bool active, void *ctxt )
{
	mainClass *classPtr;

	if( ctxt )
	{
		classPtr = static_cast<mainClass *>(ctxt);                       // Zeiger auf Objekt Casten...
		classPtr->pageChange( page, active );                            // an den richtigen Member weiterleiten
	}
}

//#############################################################################
// Externe Funktion zum erzeugen der Klarnamen
//#############################################################################
std::string __stdcall getStringForSelectionName( saSELECT _stat )
{
	std::string clearNameString;

	switch( _stat )
	{
	case saSEL_NONE:
		clearNameString = "saSEL_NONE";
		break;

	case saSEL_TOPLEFT:
		clearNameString = "saSEL_TOPLEFT";
		break;

	case saSEL_BOTTOMLEFT_LEFT:
		clearNameString = "saSEL_BOTTOMLEFT_LEFT";
		break;

	case saSEL_BOTTOMLEFT_RIGHT:
		clearNameString = "saSEL_BOTTOMLEFT_RIGHT";
		break;

	case saSEL_TOPRIGHT:
		clearNameString = "saSEL_TOPRIGHT";
		break;

	case saSEL_BOTTOMRIGHT_LEFT:
		clearNameString = "saSEL_BOTTOMRIGHT_LEFT";
		break;

	case saSEL_BOTTOMRIGHT_RIGHT:
		clearNameString = "saSEL_BOTTOMRIGHT_RIGHT";
		break;

	default:
		clearNameString = "<unknown>";
		break;
	}
	return( clearNameString );
}