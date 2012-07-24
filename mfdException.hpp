//#############################################################################
// Ableitung einer eigenen Exception Klasse f�r dieses Projekt
//#############################################################################
#pragma once

#include "saitekX52ProClass.hpp"
#include <exception>
#include <iostream>
#include <string>


//#############################################################################
// eigen Exceptionklasse  (Deklaration)
//#############################################################################
class mfdException : public std::exception
{
public:
  //mfdException(void);
  mfdException( const std::string& );                                // �berlasdener Konstruktor mit String Referenz
  //~mfdException(void);
  std::string& msg(void) const;                                      // R�ckgabe der Fehlernachricht als Referenz auf String
};
