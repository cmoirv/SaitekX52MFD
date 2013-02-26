//#############################################################################
// Implementation der Klasse für den Stick
//#############################################################################
#include "saitekX52ProClass.hpp"

using namespace std;

//#############################################################################
// Deklaration von lokalen Hilfsroutinen fuer Callbacks
//#############################################################################
void __stdcall lDeviceChange( void* device, bool added, void* ctxt );
void __stdcall lPageChange( void* device, DWORD page, bool active, void* ctxt);
void __stdcall lSoftButton( void* device, DWORD btns, void* ctxt );

//#############################################################################
// Konstruktor der Klasse
//#############################################################################
saitekX52ProClass::saitekX52ProClass(void) : 
   pLogThreadClass(0), gDevice(0), activePage(0), softButtonCb(0), pageChangeCb(0), debug(false)
{
  CDirectOutput::CDirectOutput();                                    // Das Teil Initialisieren
}

//#############################################################################
// Konstruktor der Klasse mit Zeiger auf Log Thread
//#############################################################################
saitekX52ProClass::saitekX52ProClass( logThreadClass *ptr) : 
  pLogThreadClass(ptr), gDevice(0), activePage(0), softButtonCb(0), pageChangeCb(0), debug( ptr->getDebug() )
{
  CDirectOutput::CDirectOutput();                                    // Das Teil Initialisieren
  pLogThreadClass = ptr;                                             // LogginClass merken
}                  

//#############################################################################
// Destruktor der Klasse
//#############################################################################
saitekX52ProClass::~saitekX52ProClass(void)
{
  if( debug ) logLine( "<saitekX52ProClass::~saitekX52ProClass> " );
  softButtonCb = 0;                                                  // Callback löschen
  pageChangeCb = 0;                                                  // callback löschen
  stickFuncDeinit();                                                 // DirectOutpu deinitialisieren
  CDirectOutput::~CDirectOutput();                                   // und das Objekt loeschen
  if( debug ) logLine( "<saitekX52ProClass::~saitekX52ProClass> Class destroyed" );
}

//#############################################################################
// Setze ein Logobjekt noch nachträglich
//#############################################################################
void saitekX52ProClass::setLogObject( logThreadClass *ptr )
{
  pLogThreadClass = ptr; 
}

//#############################################################################
// debuggen setzen
//#############################################################################
bool saitekX52ProClass::setDebug( bool dbg )
{
  bool old = dbg;
  debug = dbg;
  return(old);
}

//#############################################################################
// debuggen erfragen
//#############################################################################
bool saitekX52ProClass::getDebug( void )
{
  return( debug );
}

//#############################################################################
// initialisiere das Teil nach meinem Gusto
//#############################################################################
bool saitekX52ProClass::stickFuncInit( IN const wstring& appName )
{
  HRESULT hr;
  
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit>... " );
  //delAllPages();                                                     // Alle Seiten entfernen

  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> DirectOutput Init... " );
  if( hr = Initialize( (wchar_t *)appName.c_str() ) )
	{
	  if( hr == E_NOTIMPL )
	  {
        throw mfdException( "<saitekX52ProClass::stickFuncInit> Initialize fails! (Not Implementet)" );
	  }
    throw mfdException( "<saitekX52ProClass::stickFuncInit> Initialize fails!" );
	}
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> DirectOutput Init...OK" );

  // Ok, erste Hürde genommen, jetzt Registriere Callback fuer Geraetewechsel
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Register Device Change Callback... " );
	if( hr = RegisterDeviceChangeCallback( lDeviceChange, (void *)this ) )
	{
	  stickFuncDeinit();
	  if( hr == E_NOTIMPL )
	  {
        throw mfdException( "<saitekX52ProClass::stickFuncInit> Register Device Change Callback fails! (Not Implementet)" );
	  }
    throw mfdException( "<saitekX52ProClass::stickFuncInit> Register Device Change Callback fails!" );
	}
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Register Device Change Callback... OK" );

  // Weiter: Auflistung der Geraete...
	// benötigt installierten Callback "RegisterDeviceChangeCallback( &stueckClass::deviceChange, 0)"
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> enumerate Devices... " );
	if( hr = Enumerate())
	{
	  stickFuncDeinit();
	  if( hr == E_NOTIMPL )
	  {
        throw mfdException( "<saitekX52ProClass::stickFuncInit> Enumerate fails! (Not Implementet)" );
	  }
    throw mfdException( "<saitekX52ProClass::stickFuncInit> Enumerate fails!" );
	}
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> enumerate Devices... OK" );

  if( ! gDevice )
	{
	  stickFuncDeinit();
      throw mfdException( "<saitekX52ProClass::stickFuncInit> No Device Saitek X52 Pro found!" );
	}

  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Register Page Callback..." );
  // Registriere Callback zum Seitenwechsel 
  //Pfn_DirectOutput_Device_Callback
  hr = RegisterPageChangeCallback( gDevice, lPageChange, (void *)this );
  if( hr != S_OK )
	{
	  stickFuncDeinit();
	  if( hr == E_NOTIMPL )
	  {
        throw mfdException( "<saitekX52ProClass::stickFuncInit> Register Page Change Callback fails! (Not Implemented)" );
	  }
      throw mfdException( "<saitekX52ProClass::stickFuncInit> Register Page Change Callback fails!" );
	}
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Register Page Callback... OK" );

  // Registriere Callback fuer Softbuttons
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Register Soft Button Callback..." );
  hr = RegisterSoftButtonChangeCallback( gDevice, lSoftButton, (void *)this );
  if( hr != S_OK )
  {
	  stickFuncDeinit();
	  if( hr == E_NOTIMPL )
	  {
        throw mfdException( "<saitekX52ProClass::stickFuncInit> Register Softbutton Change Callback fails! (Not Implementet)" );
	  }
    throw mfdException( "<saitekX52ProClass::stickFuncInit> Register Softbutton Change Callback fails!" );
  }
  if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Register Soft Button Callback... OK" );

  // eine Grundinitialisierung sollte schon sein...
  // irgendwie muss ich mich doch verewigen.
  try
  {
	if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Add pages for MFD..." );
    addPage( ID_PAGE_SPEEDS, wstring(L"speeds"), true );
    addPage( ID_PAGE_COM1NAV1, wstring(L"com1nav1"), false );
    addPage( ID_PAGE_COM2NAV2, wstring(L"com2nav2"), false );
	addPage( ID_PAGE_NAV1, wstring(L"NAV 1"), false );// Added by Cmoirv
	addPage( ID_PAGE_NAV2, wstring(L"NAV 2"), false );// Added by Cmoirv
	addPage( ID_PAGE_QNH, wstring(L"QNH"), false );// Added by Cmoirv
	addPage( ID_PAGE_TRANSPONDER, wstring(L"Transponder"), false );// Added by Cmoirv
    setString( 0, 0, wstring( L"  X-Plane MFD   " ) );
    setString( 0, 1, wstring( L" Saitek X52 Pro " ) );
    setString( 0, 2, wstring( L"  D. Marciniak  " ) );
		
    setString( 1, 0, wstring( L"  Version 2.0  " ) );
    setString( 1, 1, wstring( L"     06/2012   " ) );
    setString( 2, 1, wstring( L"    H.LEMOINE  " ) );
    if( debug ) logLine( "<saitekX52ProClass::stickFuncInit> Add pages for MFD... ");
  }
  catch( mfdException& _ex )
  {
    throw mfdException( _ex );                                       // Exception weiterreichen
  }
  logLine( "<saitekX52ProClass::stickFuncInit>...OK " );
  return(true);
}

//#############################################################################
// eigene Deinitialisierung
//#############################################################################
bool saitekX52ProClass::stickFuncDeinit()
{
  if( debug ) logLine( "<saitekX52ProClass::stickFuncDeinit>..." );
  delAllPages();
  RegisterSoftButtonChangeCallback( gDevice, 0, 0 );
  RegisterPageChangeCallback( gDevice, 0, 0 );
  RegisterDeviceChangeCallback( 0, 0 );
  Deinitialize();
  if( debug ) logLine( "<saitekX52ProClass::stickFuncDeinit>... OK" );
  return(true);
}

//#############################################################################
// fuegt eine Seite hinzu, falls noch nicht existent
//#############################################################################
int saitekX52ProClass::addPage( int index, wstring& name, bool active )
{
  HRESULT hr;
  
  if( debug ) logLine( "<saitekX52ProClass::addPage>... " );
  if( index > STUECK_MAXPAGES-1 )
  {
    throw mfdException( "<saitekX52ProClass::addPage> Page index exceed Maximum!" );
  }
  if( pages[ index ] == 1 )
  {
    throw mfdException( "<saitekX52ProClass::addPage> Page exist, delete first!" );
  }
  if( hr = AddPage( gDevice, DWORD(index), (wchar_t *)name.c_str() , active) )
  {
    throw mfdException( "<saitekX52ProClass::addPage> Page can not added!" );
  }
  pages[index]=1;                                                    // Kennzeichne dass besetzt
  if( debug ) logLine( "<saitekX52ProClass::addPage>... OK" );
  return(index);
}

//#############################################################################
// entfernt eine Seite, falls existent
//#############################################################################
int saitekX52ProClass::delPage( int index )
{
  string msg;

  if( debug ) logLine( "<saitekX52ProClass::delPage>..." );
  if( index > STUECK_MAXPAGES-1 )
  {
    throw mfdException( "<saitekX52ProClass::delPage> Page index exceed Maximum!" );
  }
  if( S_OK != RemovePage( gDevice, index )) 
  {
    nLine.clear();
    nLine << "<saitekX52ProClass::delPage> RemovePage failed, Index: " << index;
    nLine >> msg;
    throw mfdException( msg );
  }
  pages[ index ] = 0;  
  return(index);
  if( debug ) logLine( "<saitekX52ProClass::delPage>..." );
}

//#############################################################################
// entfernt alle Seiten, ohne Rücksicht auf Ergebniswerte
//#############################################################################
bool saitekX52ProClass::delAllPages()
{
  DWORD i;
  
  if( debug ) logLine( "<saitekX52ProClass::delAllPages>..." );
  for( i=0; i<STUECK_MAXPAGES-1; i++ )
  {
    if( pages[ i ] )
    {
      try                                                            // Versuchen wir es mal...
      {
        delPage( i );
      }
      catch( mfdException& mfdEx )
      {
        errorLine( mfdEx.msg() );                                   // Es gab eine Exception! Na sowas!
      }
    }
  }
  if( debug ) logLine( "<saitekX52ProClass::delAllPages>...OK" );
  return(true);
}

//#############################################################################
// Setze String in Page (Seitennummer und Index setzen)
//#############################################################################
bool saitekX52ProClass::setString( int page, int index, std::wstring& line )
{
  HRESULT hr;

  if( pages[ page ] == 0 )
  {
    throw mfdException( "Page not existent" );
  }
  hr = SetString( gDevice, page, index, line.length(), (wchar_t *)line.c_str() );
  if( S_OK != hr  )
  {
    errorLine( "<saitekX52ProClass::SetString> schlug fehl!" );
  }
  return(true);
}
	
//#############################################################################
// Gib die ID der einsprechenden LED zurück
//#############################################################################
void saitekX52ProClass::getLedId( saLED led, int& idRed, int& idGreen )
{
  //if( debug ) logLine( "<saitekX52ProClass::getLedId>..." );
  switch( led )
  {
  case FIRE_A:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> FIRE_A" );
    idRed   = 1;
    idGreen = 2;
    break;
  case FIRE_B:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> FIRE_B" );
    idRed   = 3;
    idGreen = 4;
    break;
  case FIRE_D:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> FIRE_D" );
    idRed   = 5;
    idGreen = 6;
    break;
  case FIRE_E:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> FIRE_E" );
    idRed   = 7;
    idGreen = 8;
    break;
  case T1:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> T1" );
    idRed   = 9;
    idGreen = 10;
    break;
  case T2:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> T2" );
    idRed   = 11;
    idGreen = 12;
    break;
  case T3:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> T3" );
    idRed   = 13;
    idGreen = 14;
    break;
  case POV2:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> POV2" );
    idRed   = 15;
    idGreen = 16;
    break;
  case CLUTCH:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> CLUTCH" );
    idRed   = 17;
    idGreen = 18;
    break;
  case FIRE_BUTTON:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> FIRE_BUTTON" );
    idRed   = 0;
    idGreen = 0;
    break;
  case THROTTLE_AXIS:
    if( debug ) logLine( "<saitekX52ProClass::getLedId> THROTTLE_AIS" );
    idRed   = 19;
    idGreen = 19;
    break;
  }
  //if( debug ) logLine( "<saitekX52ProClass::getLedId>...OK" );
}

//#############################################################################
// LED Status holen
//#############################################################################
saLedStat& saitekX52ProClass::getLedStat( saLED led )
{
  return( ledStat[led] );
}

//#############################################################################
// LED Status setzen, ohne weitere Massnahmen
//#############################################################################
void saitekX52ProClass::setLedStat( saLED led, saLedStat& stat )
{
   ledStat[led] = stat;
}

//#############################################################################
// vereinfache die auswahl er Farben
//#############################################################################
void saitekX52ProClass::getVColors( saColor _color, int& _vRed, int& _vGreen )
{
  switch( _color )                                         // ID's für die Farbe suchen
  {
    case SA_OFF:
    _vRed   = 0;
    _vGreen = 0;
    break;
    case SA_RED:
    _vRed   = 1;
    _vGreen = 0;
    break;
    case SA_GREEN:
    _vRed   = 0;
    _vGreen = 1;
    break;
    case SA_AMBER:
    _vRed   = 1;
    _vGreen = 1;
    break;
    default:
      throw mfdException( "<saitekX52ProClass::makeAllLedBlink> Unknown Value for LED Color!" );
  }
}

//#############################################################################
// mache alle LED Blinkend 
//#############################################################################
bool saitekX52ProClass::makeAllLedBlink( bool alwaysSet )
{
  saLedStat localStatus;
  bool wasBlinking = false;
  int idGreen,idRed,vRed,vGreen,i;
  saLED led;
  int page = getActivePage();

  // init LED-Stati
  for( i=static_cast<int>(NONE_LED); i < static_cast<int>(FIRE_BUTTON); i++ )
  {
    // welche LED-Id's will ich eigentlich ansprechen
    led = static_cast<saLED>(i);
    getLedId( led, idRed, idGreen );
    getVColors( ledStat[ led ].first, vRed, vGreen );                          // Farben codieren
    // wenn es blinken soll UND nicht aus ist ODER wenn es IMMER soll!
    if( ((ledStat[ led ].second == SA_BLINK) && (ledStat[ led ].first != SA_OFF)) || alwaysSet )   
    {
      if( !onPhase )                                                           // während der Dark-Phase
      {
        vRed   = 0;
        vGreen = 0;
      }
    }
    SetLed( gDevice, page, idRed,   vRed );                                    // Rot setzen
    SetLed( gDevice, page, idGreen, vGreen );                                  // Grün setzen
    if( ledStat[ led ].second == SA_BLINK ) wasBlinking = true;                // ok, war ein Blinker!
  }
  if( wasBlinking ) onPhase = !onPhase;                                        // Phasenwechsel
  // die letzen blinken NIE
  if( alwaysSet )
  {
    if( ledStat[ FIRE_BUTTON ].first == SA_OFF )
      SetLed( gDevice, page, 0, 0 );
    else
      SetLed( gDevice, page, 0, 1 );
    
    if( ledStat[ THROTTLE_AXIS ].first == SA_OFF )
      SetLed( gDevice, page, 19, 0 );
    else
      SetLed( gDevice, page, 19, 1 );
    return( true );
  }
  return( wasBlinking );
}

//#############################################################################
// LED setzen...
// 	HRESULT SetLed( IN void* hDevice, IN DWORD dwPage, IN DWORD dwIndex, IN DWORD dwValue)
//#############################################################################
int saitekX52ProClass::setLed(int page, saLED led, saColor value, saBlink bl )
{
  int idGreen,idRed;
  string msg;
  
  //if( debug ) logLine( "<saitekX52ProClass::setLed>..." );
  if( bl == SA_BLINK ) msg = "BLINK"; else msg = "NOBLINK";
  if( led < FIRE_BUTTON )
  {
    // welche LED-Id's will ich eigentlich ansprechen
    getLedId( led, idRed, idGreen );
    switch( value )
    {
      case SA_OFF:
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_OFF; " + msg + "..." );
      SetLed( gDevice, page, idRed,   0 );
      SetLed( gDevice, page, idGreen, 0 );
      break;
      case SA_RED:
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_RED; " + msg + "..." );
      SetLed( gDevice, page, idRed,   1 );
      SetLed( gDevice, page, idGreen, 0 );
      break;
      case SA_GREEN:
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_GREEN; " + msg + "..." );
      SetLed( gDevice, page, idRed,   0 );
      SetLed( gDevice, page, idGreen, 1 );
      break;
      case SA_AMBER:
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_AMBER; " + msg + "..." );
      SetLed( gDevice, page, idRed,   1 );
      SetLed( gDevice, page, idGreen, 1 );
      break;
      default:
        throw mfdException( "<saitekX52ProClass::setLed> Unknown Value for LED Color!" );
    }
    ledStat[led].first = value;
    ledStat[led].second = bl;
    //if( debug ) logLine( "<saitekX52ProClass::setLed>...OK" );
    return( value );
  }
  if( led == FIRE_BUTTON )
  {
    if( value == SA_OFF )
    {
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_OFF..." );
      SetLed( gDevice, page, 0, 0 );
    }
    else
    {
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_ON..." );
      SetLed( gDevice, page, 0, 1 );
    }
    ledStat[led].first = value;
    ledStat[led].second = bl;
    //if( debug ) logLine( "<saitekX52ProClass::setLed>...OK" );
    return(value);
  }
  if( led == THROTTLE_AXIS )
  {
    if( value == SA_OFF )
    {
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_OFF..." );
      SetLed( gDevice, page, 19, 0 );
    }
    else
    {
      if( debug ) logLine( "<saitekX52ProClass::setLed> SA_ON..." );
      SetLed( gDevice, page, 19, 1 );
    }
    ledStat[led].first = value;
    ledStat[led].second = bl;
    //if( debug ) logLine( "<saitekX52ProClass::setLed>...OK" );
    return( value );
  }
  throw mfdException( "<saitekX52ProClass::setLed> Unknown Value for LED!" );
  return(0);
}

//#############################################################################
// Daten ins Logfile kitten, dabei formatieren
//#############################################################################
bool saitekX52ProClass::logLine( std::string& line )
{
  if( pLogThreadClass )
    return( pLogThreadClass->logLine( line ) );
  return(false);
}

//#############################################################################
// Daten ins Logfile kitten, dabei formatieren (auch mit char* )
//#############################################################################
bool saitekX52ProClass::logLine( char *line )
{
  if( pLogThreadClass )
    return( pLogThreadClass->logLine( line ) );
  return(false);
}

//#############################################################################
// Fehlerausgabe mit *char
//#############################################################################
bool saitekX52ProClass::errorLine( char *line )
{
  if( pLogThreadClass )
    return( pLogThreadClass->errorLine( line ) );
  return(false);
}

//#############################################################################
// Fehlerausgabe mit string
//#############################################################################
bool saitekX52ProClass::errorLine( std::string& line )
{
  if( pLogThreadClass )
    return( pLogThreadClass->errorLine( line ) );
  return(false);
}

//#############################################################################
// Gerät geändert Callback
//#############################################################################
void saitekX52ProClass::deviceChange( void* device, bool added )
{
  if( debug ) logLine( "<saitekX52ProClass::deviceChange> Callback: DEVICE CHANGE" );
	if( gDevice == 0 && added )
  {
		gDevice = device;
    if( debug ) logLine( "<saitekX52ProClass::deviceChange> Device Changed!" );
  }
	else 
	  if( !added && gDevice == device )
    {
		  gDevice = 0;
      if( debug ) logLine( "<saitekX52ProClass::deviceChange> Device always added!" );
    }
}

//#############################################################################
// Callback für Page Change
//#############################################################################
void saitekX52ProClass::pageChange( void* device, DWORD page, bool active )
{
  char buffer[255];

  if( active ) activePage = int( 0xffffffff & page); 
  if( pageChangeCb )
  {
    pageChangeCb( page, active, pageChangeCtx );
    return;
  }
  sprintf_s( &buffer[0], 255, "<saitekX52ProClass::pageChange> callback: PAGE CHANGE, Page %d, %s\0", int( 0xffffffff & page ), (active ? "true" : "false") );
  logLine( buffer );
}

//#############################################################################
// Callback für Soft Button
//#############################################################################
void saitekX52ProClass::softButton( void* device, DWORD btns )
{
  char buffer[255];
  string outPut;

  // So, jetz wird es kompliziert...
  // Wenn ich einen Callback für den Callback eingerichtet habe, 
  // dann benutze ish einfach mal diesen, ansonste schreib einfach
  // bei debug stupide in die Logdatei
  if( softButtonCb )
  {
    softButtonCb( btns, softButtonCtx );                             // callback anrufen
    return;                                                          // das war es!
  }
  if( debug )
  {
    sprintf_s( &buffer[0], 255, "<saitekX52ProClass::softButton> Callback: SOFT BUTTON, Page %d, \0", activePage );
    logLine( buffer );
    outPut = string( &buffer[0] );
    //logLine( buffer );

	  if( btns & SoftButton_Select )
    {
	    outPut +=  " SELECT ";
    }
    if( btns & SoftButton_Up )
    {
	    outPut += " UP ";
    }
    if( btns & SoftButton_Down )
    {
	    outPut += " DOWN ";
    }
    if( btns & SoftButton_Left )
    {
	    outPut += " LEFT ";
    }
     if( btns & SoftButton_Right )
    {
	    outPut += " RIGHT ";
    }
    if( btns & SoftButton_Back )
    {
	    outPut += " BACK ";
    }
    if( btns & Softbutton_Increment )
    {
	    outPut += " INCR ";
    }
    if( btns & SoftButton_Decrement )
    {
     outPut += " DECR ";
    }
    logLine( outPut );
  }
}

//#############################################################################
// Gib aktive Page zurück
//#############################################################################
int saitekX52ProClass::getActivePage(void)
{
  return( activePage );
}

//#############################################################################
// Gib aktive Page zurück und setze neue Seite
//#############################################################################
int saitekX52ProClass::setActivePage( int page )
{
  int old = activePage;
  activePage = page;
  return( old );
}

//#############################################################################
// Registriere eigene Callback-Funktion dafür
//#############################################################################
bool saitekX52ProClass::registerSoftButtonCalback( dmSoftButtonCallback *func, void *ctxt )
{
  softButtonCb = func;
  softButtonCtx = ctxt;
  return( true );
}

//#############################################################################
// Registriere eigene Callback-Funktion dafür
//#############################################################################
bool saitekX52ProClass::registerPageChangeCallback( dmPageChangeCallback *func, void *ctxt)
{
  pageChangeCb = func;
  pageChangeCtx = ctxt;
  return(true);
}

//#############################################################################
//#############################################################################
//#############################################################################
//#############################################################################

//#############################################################################
// Lokale Hilfsroutine zum Weiterleiten des Calls an das entsprechende Object
//#############################################################################
void __stdcall lDeviceChange( void* device, bool added, void* ctxt )
{
  saitekX52ProClass *classPtr;

  if( ctxt )
  {
    classPtr = static_cast<saitekX52ProClass *>(ctxt);               // Zeiger auf Objekt Casten...
    classPtr->deviceChange( device, added );                         // an den richtigen Member weiterleiten
  }
}

//#############################################################################
// Lokale Hilfsroutine zum Weiterleiten des Calls an das entsprechende Object
//#############################################################################
void __stdcall lPageChange( void* device, DWORD page, bool active, void* ctxt)
{
  saitekX52ProClass *classPtr;
  
  if( ctxt )
  {
    classPtr = static_cast<saitekX52ProClass *>(ctxt);               // Zeiger auf Objekt Casten...
    classPtr->pageChange( device, page, active );                    // an den richtigen Member weiterleiten
  }
}

//#############################################################################
// Lokale Hilfsroutine zum Weiterleiten des Calls an das entsprechende Object
//#############################################################################
void __stdcall lSoftButton( void* device, DWORD btns, void* ctxt )
{
  saitekX52ProClass *classPtr;

  if( ctxt )
  {
    classPtr = static_cast<saitekX52ProClass *>(ctxt);               // Zeiger auf Objekt Casten...
    classPtr->softButton( device, btns );                            // an den richtigen Member weiterleiten
  }
}

