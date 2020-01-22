//////////////////////////////////////////////////////
// CGI stdin/query parser source file.
// (c) Jon DuBois 2004
// 02/27/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
//////////////////////////////////////////////////////
#include <cstring>
#include <cstdlib>
#include <cstddef>
#include <exception>
#include "bcj/string.H"
#include "bcj/assess.H"
#include "bcj/cgiparse.H"
#include "bcj/gregex.H"
#include "bcj/stack.H"
#include "bcj/algo.H"
using namespace std; 


//Put this here to handle constructer throws
void UEHandler( void ){
  cout << "Content-type: text/plain\n\n";
  cout << "An exception occured while processing a cgi directive, please"
    " contact the author (Jon DuBois, bcj1980@sbcglobal.net) and let him know"
    " the circumstances of the failure if possible.";
  exit( EXIT_FAILURE );
}
struct Init{
  Init( void ){
    set_terminate( UEHandler );
  }
}; 
Init init;


/////////////////////////////////////////////////////
// CGIParse.
/////////////////////////////////////////////////////
// class CGIParse{
  
//   struct Imp;
//   Imp* imp;
//   // No copying or assigning
//   CGIParse& operator=( const CGIParse& );
//   CGIParse( const CGIParse& );

// public:
  
//   CGIParse( void );
//   ~CGIParse( void );

//   size_t size( void ) const;

//   // these may be treated as arrays containing the variables
//   const String& name( size_t ind ) const;
//   const String& value( size_t ind ) const;
//   static String env( const String& );
//   const String& raw( void ) const;
//   // This returns false if the input varaible names do not match this string.
//   bool validate( const String& ) const;
// };
                                                                                   
struct CGIParse::Imp{
 
  Stack< Pair< String, String > > values;
  static GRegex cgire;
  static GRegex vre;
  size_t size;
  String rstr;

  Imp( void );
  ~Imp( void );
  
  // Treats rstr as x-www-form-urlencoded
  void urldecode( void );
  // Treats rstr as multipart/form-data with boundary b
  void mpdecode( const String b );
};
GRegex CGIParse::Imp::cgire( "([^=]*)=([^&]*)&?" );  
GRegex CGIParse::Imp::vre( "(^|/?)([^?]+)(/?|$)" );


inline CGIParse::Imp::Imp( void ) : size( 0 ){
  String rm( env( "REQUEST_METHOD" ) );
  rm.upcase();

  if( rm == String( "GET" ) ){
    rstr = env( "QUERY_STRING" );
    urldecode();
  }
  else if( rm == String( "POST" ) ){
    String ct( env( "CONTENT_TYPE" ) );
    rstr = String( cin );
    String dct( ct );
    dct.downcase();
    dct.regex( "^([^;]*)(;[^;]*)*$", "\\1" );
    if( dct == String( "application/x-www-form-urlencoded" ) )
      urldecode();
    else if( dct == String( "multipart/form-data" ) ){
      ct.regex( "boundary *=\"?([^\"]*)\"?$", "\\1" );
      if( !ct.size() )
	throw cgiparse_error( "No MIME type supplied in post" );
      mpdecode( ct );
    }
    else
      throw cgiparse_error( "Unrecognized MIME type in post" );
  }
  else
    throw cgiparse_error( "Unrecognized request method" );
}
inline CGIParse::Imp::~Imp( void ) {}


void CGIParse::Imp::urldecode( void ){
  cgire.search( rstr );
  while( cgire.found() ){
    Pair< String, String > q( cgire[ 1 ], cgire[ 2 ] );
    values.push( q ); 
    cgire.next();
    size++;
  }
  if( size > 1 )
    Sort( values );
}
void CGIParse::Imp::mpdecode( const String b ){
  rstr.reset();

  GRegex bre( "^--" + b ); 
  String s;
  String d( "--" + b + "--" );

  s = rstr.nextline();
  bre.search( s );
  while( !bre.found() && s.more() ){
    s = rstr.nextline(); 
    bre.search( s ); 
  }
  while( 1 ){
    if( !s.more() || d.prefix( s ) )
      break;
 
    static GRegex cdre( "^[Cc][Oo][Nn][Tt][Ee][Nn][Tt]"
			"-[Dd][Ii][Ss][Pp][Oo][Ss][Ii][Tt][Ii][Oo][Nn]:[ \t]*"
			"[Ff][Oo][Rr][Mm]-[Dd][Aa][Tt][Aa]" );
    s = rstr.nextline();
    cdre.search( s );
    while( !cdre.found() && s.size() ){
      s = rstr.nextline();
      cdre.search( s );
    }
    if( !cdre.found() )
      throw cgiparse_error( "Content-Dispostion not found in" 
		       " body part of multipart/form-data" );
    // Unfold the header

    String t;
    t = s;
    s = rstr.nextline();
    while( ( s[ 0 ] == '\t' ) || ( s[ 0 ] == ' ' ) ) { 
      t.put( s, s.size() - 1, 1 );
      s = rstr.nextline();
    }
    // t now has the content-disposition
    t.regex( ";[ \t]*[Nn][Aa][Mm][Ee][ \t]*=[ \t]*\"?([^\";]+)" , "\\1" );
    // t now is the name
    while( s.size() ){
      s = rstr.nextline();
    }
    s = rstr.nextline();

    bre.search( s );
    String z;
    z.erase();
    String a;
    a.erase();
    while( !bre.found() && rstr.more() ){
      a += z;
      z = rstr.lastlinebreak();
      a += s;
      s = rstr.nextline();
      bre.search( s );
    }
    size++;
    values.push( Pair< String, String >( t, a ) ); 
  }
  if( size > 1 )
    Sort( values );
}
 


CGIParse::CGIParse( void ) : imp( new Imp ) {}
CGIParse::~CGIParse( void ) {
  delete imp;
}


size_t CGIParse::size( void ) const{
  return imp->size;
}


const String& CGIParse::name( size_t ind ) const{
  assess( ind < imp->values.size(), "Index out of bounds for name in a CGIParse" );
  return imp->values[ ind ].key;
}
const String& CGIParse::value( size_t ind ) const{
  assess( ind < imp->values.size(), "Index out of bounds for value in a CGIParse" );
  return imp->values[ ind ].value;
}
String CGIParse::env( const String& nm ){
  const char* c = getenv( nm.ccptr() );
  if( c )
    return String( c );
  else
    return String();
}
const String& CGIParse::raw( void ) const{
  return imp->rstr;
}
bool CGIParse::validate( const String& val ) const{
  imp->vre.search( val );
  size_t i = 0;
  while( imp->vre.found() && ( i < imp->size ) ){
    if( imp->vre[ 2 ] != imp->values[ i ].key )
      return false;
    imp->vre.next();
    i++;
  }
  if( imp->vre.found() || ( i != imp->size ) )
    return false;
  return true;
}
