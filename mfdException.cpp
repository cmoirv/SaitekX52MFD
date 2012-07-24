//#############################################################################
// Implementation der eigenen Exception Klasse für dieses Projekt
//#############################################################################
#include "mfdException.hpp"

//#############################################################################
// Überladener Konstruktor
//#############################################################################
mfdException::mfdException( const std::string& _msg)
{
  exception( _msg.c_str() );
  //_m_what = _msg.c_str();
}

std::string& mfdException::msg(void) const
{
  static std::string msg;

  msg = std::string( what() );
  return( msg );
}
