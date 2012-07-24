//#############################################################################
// Deklaration der Klasse logThradClass, loggt in eigenem Thread
// um den Simulator zu entlasten und nicht zu blockieren
//#############################################################################

#include "logThreadClass.hpp"
#include <time.h>

using namespace std;

DWORD WINAPI startLogThreadFunc(void*);                              // Hilfsfunkttion zum Starten...


//#############################################################################
// der Konstruktor (Standart) Private!
//#############################################################################
logThreadClass::logThreadClass(void)
{
}

//#############################################################################
// Konstruktor, mit �bergabe des Logfilenamens
//#############################################################################
logThreadClass::logThreadClass( string& logFileName ): oStream(0),threadHandle(0),
                threadId(0),threadIsRunning(false),debug(false)
{
  _init();                                                           // nur zur �bersichtlichkeit ausgelagert
  try                                                                // MAcht sie Sache sicherer
  {
    stringList.clear();                                              // die Liste (eigentlich ja vector) sicherheitshalber leeren
    oStream = new ofstream();                                        // Datei-Strom erzeigen
    oStream->open( logFileName.c_str(), (ios::out|ios::app) );       // zum Anf�gen �ffnen
    *oStream << endl << _formatLogString( string(" ") ) << endl;     // kleinen optischen Abstand schaffen
    resumeThread();                                                  // den Schreiberling-Thread aufwecken
    flushStream();                                                   // Puffer leeren
  }
  catch( char * )                                                    // bei Exceptions reagieren
  {
    oStream = 0;
  }
}

//#############################################################################
// Hilfsfunktion zum erzeugen eines Threads...
//#############################################################################
void logThreadClass::_init(void)
{
  logStructMutex = CreateMutex( 0, true, 0 );                        // Mutex f�r Log-schreiben
  threadIsRunning = true;                                            // Kennzeichne als laufend...  
  threadHandle = CreateThread( 
                      0,                                             // security  
                      0x1000,                                        // Stack Size 
                      startLogThreadFunc,                            // Adresse des Threads
                      this,                                          // Thread Argumenteliste
                      CREATE_SUSPENDED,                              // create suspended
                      &threadId);                                    // Thread ID
  SetPriorityClass(
                     threadHandle,
                     BELOW_NORMAL_PRIORITY_CLASS);                   // sei gef�lligst nett zum System
  ReleaseMutex( logStructMutex );
}


//#############################################################################
// Zerst�re einfach die p�se Klasse!
//#############################################################################
logThreadClass::~logThreadClass(void)
{
  if( debug )
    logLine( "STOP LogThreadClass" );
  while( resumeThread() > 0 ) ;                                      // falls der schlafen ist...
  Sleep( 200 );                                                      // da k�nnte man noch den Puffer abarbeiten
  threadIsRunning = false;                                           // Stop Bedingung stellen
  WaitForSingleObject( logStructMutex, INFINITE );                   // Warte, bis der Mutex dem Thread nicht mehr geh�rt (releaseMutex)
  CloseHandle( logStructMutex );                                     // lass den Mutex auch verschwinden
  if( oStream ) delete oStream;
}

//#############################################################################
// debuggen setzen
//#############################################################################
bool logThreadClass::setDebug( bool dbg )
{
  bool old = dbg;
  debug = dbg;
  return(old);
}

//#############################################################################
// debuggen erfragen
//#############################################################################
bool logThreadClass::getDebug( void )
{
  return( debug );
}

//#############################################################################
// Logstring formatieren
//#############################################################################
string logThreadClass::_formatLogString( string& line )
{
  static unsigned int lineCount = 0;                                 // ich nummeriere gern die Zeilen...
  tm tmFields;                                                       // f�r localtime
  time_t tim = time(0);                                              // wie sp�t isses bitte?
  string resultString;    
  char buffer[255];
  int chars;

  // Sicherstellen, dass der Puffer nicht �berl�uft
  if( line.length() > (255-22) )
  {
    resultString = line.substr(0, (255-22) );
    line = resultString;
  }
  // Lokalzeit codieren
  localtime_s( &tmFields, &tim );
  chars = sprintf_s( buffer, 254, 
                   "[%02d.%02d.%04d %02d:%02d:%02d] [%08u] %s\n\0" ,
                   tmFields.tm_mday, tmFields.tm_mon, (tmFields.tm_year + 1900),
                   tmFields.tm_hour, tmFields.tm_min, tmFields.tm_sec,
                   lineCount++, 
                   line.c_str() );
  resultString = string( &buffer[0] );
  return( resultString );
}

//#############################################################################
// Daten ins Logfile kitten, dabei formatieren
//#############################################################################
bool logThreadClass::logLine( std::string& line )
{
  if( ! threadIsRunning ) return( false );
  // um den Simulator nicht zu verz�gern, lieber eine Zeile vermissen und nicht ZU lange warten...
  if( WAIT_OBJECT_0 == WaitForSingleObject( logStructMutex, 200 ) )    // Warte, bis der Thread mal Zeit hat...
  {
    stringList.insert( stringList.end(), _formatLogString( line ) );   // f�ge den String in die Liste ein
    //stringList.push_back( _formatLogString( line ) );                // Formatierten String in die Liste
    ReleaseMutex( logStructMutex );                                    // Sag dem Write Thread, er kann wieder hier ackern
  }
  return(true);
}

//#############################################################################
// Daten ins Logfile kitten, dabei formatieren (auch mit char* )
//#############################################################################
bool logThreadClass::logLine( char *line )
{
  string newLine = line;                                             // umwandeln in string
  return( logLine( newLine ) );
}

//#############################################################################
// Fehlerausgabe mit *char
//#############################################################################
bool logThreadClass::debugLine( char *line )
{
  if( ! debug ) return(false);
  string newLine = line;
  return( errorLine( newLine ) );
}

//#############################################################################
// Fehlerausgabe mit string
//#############################################################################
bool logThreadClass::debugLine( std::string& line )
{
  if( ! debug ) return(false);
  string newLine = string("DEBUG: ") + line;
  return( logLine( newLine ) );
}

//#############################################################################
// Fehlerausgabe mit *char
//#############################################################################
bool logThreadClass::errorLine( char *line )
{
  string newLine = line;
  return( errorLine( newLine ) );
}

//#############################################################################
// Fehlerausgabe mit string
//#############################################################################
bool logThreadClass::errorLine( std::string& line )
{
  string newLine = string("ERROR: ") + line;
  return( logLine( newLine ) );
}


//#############################################################################
// Outstream flushen!
//#############################################################################
void logThreadClass::flushStream(void)
{
  if( ! oStream ) return;
  *oStream << flush;
}

//#############################################################################
// Der eigentliche Thread, l�uft, bis threadIsRunning auf False gesetzt wird
// TODO: Den sonstigen mutex k�nnt ich verschwinden lasen, brauch ich eh nicht
//#############################################################################
void logThreadClass::theThread( void *par )
{
  DWORD dwWaitResult;                                                // Resultat der Wartefunktion vom Mutex
  time_t tim = time(0);                                              // wie sp�t isses bitte?

  //mach was...
  if( debug ) logLine( "<logThreadClass::theTread>: Thread started.\n" );
  while( threadIsRunning )                                           // L�uft solange, wie dese Variable auf TRUE gesetzt bleibt
  {
    if( ! oStream ) SuspendThread( threadHandle );                   // geh schlafen, ist eh nix zu tun!
    if( tim+1 >= time(0) )                                           // flush Zeit?
    {
      tim = time(0);                                                 // Zeit Reset
      if( oStream )
        oStream->flush();                                            // Stream flushen!!!!
    }
    // Warte, ob ein Objekt verf�gbar ist...
    dwWaitResult = WaitForSingleObject( logStructMutex, INFINITE  );
    if( dwWaitResult == WAIT_OBJECT_0 )                              // Ist es das, worauf ich wartete?
    {
      if( stringList.empty() )                                       // Da ist nix drin!
      {
        ReleaseMutex( logStructMutex );                              // Freigeben!
        Sleep( 100 );                                                // 100 ms warten
        continue;                                                    // weiter!
      }
      if( oStream )                                                  // auch nur, wenn ein Stream da ist
      {
        *oStream << stringList.front();                              // dann ausgeben!!!!!
        stringList.erase( stringList.begin() );                      // Ausgabe k�nne man nich weiter puffern?
      }
      else
        Sleep( 1000 );                                               // Kein Objekt, kann ich auch l�nger pennen! (und hoffen, da� einer ein Objekt erstellt)
      ReleaseMutex( logStructMutex );                                // Besitz Freigeben!
      continue;
    }
    else if( dwWaitResult > WAIT_ABANDONED_0 )                       // Ist es ein "Herrenloses" Objekt gewesen?
    {
      if( debug )
      {
        if( oStream ) 
        {
          *oStream << "ERROR " << __FILE__ << " " << __LINE__ << " abandoned WAIT!" << endl;
          oStream->flush();
        }
      }
      _endthreadex( -1 );                                            // und tsch��!
      return;
    }
  }
  if( oStream && debug ) *oStream << _formatLogString( string( "<logThreadClass::theTread>: Thread stopping.\n" ) );
  ReleaseMutex( logStructMutex );                                    // Besitz abgeben
  _endthreadex( 1 );                                                 // unt tsch��!
  return;                                                            // und tsch��!
}

//#############################################################################
// Memberfunktion, die den Thread erst mal schlafen schickt
//#############################################################################
int logThreadClass::suspendThread( void )
{
  DWORD res;

  res = SuspendThread( threadHandle );
  if( res == -1 )
    return( false );
  return( int( res & 0xffff ) );
}

//#############################################################################
// Memberfunktion die den Thread aufweckt
//#############################################################################
int logThreadClass::resumeThread(void)
{
  DWORD res;

  res = ResumeThread( threadHandle );
  if( res == -1 )
    return( false );
  return( int( res & 0xffff ) );
}

//#############################################################################
//#############################################################################
// Einstiegsstelle des Threads...
// nicht Bestandteil des Objektes
//#############################################################################
//#############################################################################
DWORD WINAPI startLogThreadFunc(void *ptr)
{
  logThreadClass *classPtr;

  classPtr = static_cast<logThreadClass*>(ptr);                      // zun�chst mal die Adresse des Objektes casten
  classPtr->theThread(0);                                            // und dann Member aufrufen!
  return(1);
}
