//#############################################################################
// Implementation der eigenen Exception Klasse f�r dieses Projekt
//#############################################################################
#include "mfdException.hpp"

//#############################################################################
// �berladener Konstruktor
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
