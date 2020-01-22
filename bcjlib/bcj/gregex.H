////////////////////////////////////////////////
// GNU regex libraru wrapper header
// (c) Jon DuBois 2004
// 02/28/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef GREGEX_H
#define GREGEX_H
#include <cstddef>
#include "bcj/assess.H"
#include "bcj/cregex"
#include "bcj/string.H"



/////////////////////////////////////////////
// Exceptions
/////////////////////////////////////////////
struct regex_error : public run_error{
  regex_error( const char* msg = "General regular expression error" ) : run_error( msg ){}
};



/////////////////////////////////////////////
// GRegex
/////////////////////////////////////////////
class GRegex{

  struct Imp;
  Imp* imp;
  // No copying or assigning.
  GRegex( const GRegex& );
  GRegex& operator=( const GRegex& );

public:

  GRegex( const String& );
  ~GRegex( void );

  bool search( const String& );
  bool next( void );
  size_t subexps( void ) const;
  bool found( void ) const;
  
  String operator[]( size_t ) const;
  bool operator!( void ) const;
};



#endif //GREGEX_H

