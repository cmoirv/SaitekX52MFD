//#############################################################################
// Deklaration der Klasse logThradClass, loggt in eigenem Thread
// um den Simulator zu entlasten und nicht zu blockieren
//#############################################################################

#pragma once

#include "projectCommon.h"
#include <process.h>    /* _beginthread, _endthread */
#include <stddef.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>


typedef std::vector< std::string > stlStringList;                      // Definiere einen Typ f�r mich

//#############################################################################
// Deklaration der Klasse zum Loggen in eine Datei mittels eigenem thread
//#############################################################################
class logThreadClass
{
private:
  logThreadClass(void);                                              // Standartkonstruktor nicht m�glich
public:
  logThreadClass( std::string& );                                    // Konstruktor mit Reerenz auf Dateinamen String
  ~logThreadClass(void);                                             // der Zerst�rer
  void theThread( void *par );                                       // der eignetlihe Thread
  int suspendThread(void);                                           // Thread schlafen schicken
  int resumeThread(void);                                            // Thread aufwecken
  bool logLine( std::string& line );                                 // Datenzeile in die Logdatei schreiben
  bool logLine( char *line );                                        // Datenzeile in die Logdatei schreiben
  bool debugLine( std::string& line );                               // Datenzeile in die Logdatei schreiben
  bool debugLine( char *line );                                      // Datenzeile in die Logdatei schreiben
  bool errorLine( std::string& line );                               // Datenzeile in die Logdatei schreiben
  bool errorLine( char *line );                                      // Datenzeile in die Logdatei schreiben
  void flushStream(void);                                            // Dateipuffer schreiben
  bool setDebug( bool dbg );                                         // Debugging setzen...
  bool getDebug( void );                                             // wie war das noch?
private:
  bool threadIsRunning;                                              // daran sol der Thread merken, wann er suizid begehen soll
  void _init(void);                                                  // interne Init Funktion, der �bersichtlichkeit halber
  std::string _formatLogString( std::string& );                      // Formatiere den String 
  std::ofstream *oStream;                                            // ausgabestream f�r Logging
  stlStringList stringList;                                          // eine Liste von Strings...
  HANDLE logStructMutex;                                             // Regelt den Zugriff auf die Liste mit Strings f�r LOG
  HANDLE threadHandle;                                               // der Griff am Thread...
  DWORD threadId;                                                    // die ID des Thread (brauch ich den?)
  bool debug;                                                        // hier mekr ich mir das einfach mal
};
