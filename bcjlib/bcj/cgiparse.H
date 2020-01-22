////////////////////////////////////////////////
// CGI stdin parser header
// (c) Jon DuBois 2004
// 03/3/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef CGIPARSE_H
#define CGIPARSE_H
#include <iostream>
#include "bcj/string.H"
#include "bcj/cowa.H"
#include "bcj/assess.H"



////////////////////////////////////////////////
// Exceptions
////////////////////////////////////////////////
struct cgiparse_error : public run_error{
  cgiparse_error( const char* msg = "Unspecified error while parsing CGI stream" ) : run_error( msg ) {}
};



/////////////////////////////////////////////
// CGIParse
/////////////////////////////////////////////
class CGIParse{
  
  struct Imp;
  Imp* imp;
  // No copying or assigning
  CGIParse& operator=( const CGIParse& );
  CGIParse( const CGIParse& );

public:
  
  CGIParse( void );
  ~CGIParse( void );

  size_t size( void ) const;

  // these may be treated as arrays containing the variables
  const String& name( size_t ind ) const;
  const String& value( size_t ind ) const;
  static String env( const String& );
  const String& raw( void ) const;
  // This returns false if the input varaible names do not match this string.
  bool validate( const String& ) const;
};
                                                                                   


#endif //CGIPARSE_H
