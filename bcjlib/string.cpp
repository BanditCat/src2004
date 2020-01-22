//////////////////////////////////////////////////////
// String source file.
// (c) Jon DuBois 2004
// 02/27/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
//
// No, I'm not secretly a masachist.... The reason I'm
// re-implementing strings is that the one that came
// with my compiler had a memory leak.
//
// TODO:
// Make it so that the string is always null 
// terminated so that putting the trailing zero is
// unnessecary.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include "bcj/string.H"
#include "bcj/assess.H"
#include "bcj/cowa.H"
#include "bcj/gregex.H"
using namespace std; 



/////////////////////////////////////////////////////
// String.
///////////////////////////////////////////////////// 
// class String{ 

// public:

//   String( void );
//   String( const String& );
//   // Substring copy constructor.
//   String( const String&, size_t start, size_t length );
//   String( const char* );
//   // For non-NULL terminated strings of known length
//   String( const char*, size_t );
//   // Creates a string of the same character of length n.
//   String( char, size_t = 1 );
//   String( std::istream& );
//   String( int );
//   String( size_t );
//   ~String( void );
//   String& operator=( const String& ); 
//   // This takes ownership of a char array, including deleting it 
//   // when finished, the size given must include the terminating null.
//   void take( char*, size_t sz );

//   const char* ccptr( void ) const;
//   size_t size( void ) const;
//   void put( char c );
//   void put( const char*, size_t len, size_t sind = 0 );
//   void put( const String& );
//   void put( const String&, size_t len, size_t sind = 0 );
//   void erase( void );
//   void substitute( char oldc, char newc );
//   void upcase( void );
//   void downcase( void );

//   // Returns the next line of s, or nothing
//   String nextline( void ) const;
//   // This resets the postion
//   void reset( void ) const;
//   // This is true iff there is more to be read
//   bool more( void ) const;
//   // This returns the last linebreak( eg. \n, \r, or \r\n ).
//   String lastlinebreak( void ) const; 

//   // This %-decodes the string, i.e. for CGI
//   void urldecode( void );
//   // This turns <>&" into html entities, i.e. for embedding in html 
//   void htmlize( void );
//   // This compresses all whitespace down to a single space
//   void flatten( void );
//   // This turns a string into a paragraph of width wid, indenting the first line
//   // flind + ind, and all other lines ind
//   void paragraph( size_t wid, size_t ind = 0, size_t flind = 0 );

//   bool isspace( void ) const;
//   bool isalnum( void ) const;
//   bool contains( char c ) const;
//   // True if the string is a prefix of str
//   bool prefix( const String& str ) const;

//   String& operator+=( const String& ); 
//   String& operator*=( size_t mul );
//   const String operator+( const String& ) const;
//   const String operator*( size_t mul ) const;
 
//   bool operator>( const String& ) const;
//   bool operator>=( const String& ) const;
//   bool operator<( const String& ) const;
//   bool operator<=( const String& ) const;

//   char operator[]( size_t ) const;
//   bool operator!( void ) const;
//   bool operator==( const String& ) const;
//   bool operator!=( const String& ) const;
//   // Appends the next contiugously non-whitespace region of an istream onto 
//   // the back of a String.
//   friend std::istream& operator>>( std::istream&, String& );
//   friend std::ostream& operator<<( std::ostream&, const String& );
//   friend const String operator+( const char*, const String& );
//   friend const String operator+( const char, const String& );

// private:
  
//   // The last line terminator
//   mutable enum { lf, crlf, cr, none } lastlb;
//   // Read position within string
//   mutable size_t readpos;
//   // Size NOT including null, e.g. a zero-length string has size 0.
//   size_t sz;
//   // Size of buffer.
//   size_t max;
//   // This is mutable, I prefer const cast away over the mutable keyword
//   Cowa< char > cstr;
  
// };
 


String::String( void ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){}
String::String( const String& cp ) : sz( cp.sz ), max( cp.max ), cstr( cp.cstr ), readpos( 0 ){}
String::String( const String& cp, size_t strt, size_t len ) : sz( len ), max( len + 1 ), 
							       cstr( cp.cstr + strt, len, len + 1 ), readpos( 0 ){
  assess( ( strt + len ) < cp.max, "Attempt to copy a faulty String substring" );
}
String::String( int arg ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){
  char ta[ 64 ]; 
  int i = snprintf( ta, 63, "%d", arg );
  assess( i < 64, "Buffer overflow in String::String( int )" );
  const char* tb = ta;
  cstr = Cowa< char >( tb, 64, 64 );
  max = 64;
  sz = i;
} 
String::String( size_t arg ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){
  char ta[ 64 ];
  int i = snprintf( ta, 63, "%u", arg );
  assess( i < 64, "Buffer overflow in String::String( int )" );
  const char* tb = ta;
  cstr = Cowa< char >( tb, 64, 64 );
  max = 64;
  sz = i;
} 
String::String( const char* cp ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){
  if( !cp ) 
    throw string_error( "Attempt to call String constructor on a NULL pointer" );
  size_t ssz = strlen( cp );
  if( ssz ){ 
    Cowa< char > tc( cp, ssz + 1, ssz + 1 );
    max = ( sz = ssz ) + 1;
    cstr = tc;
  } 
}
String::String( const char* cp, size_t ssz ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){
  if( !cp ) 
    throw string_error( "Attempt to call String constructor on a NULL pointer" );
  if( ssz ){ 
    Cowa< char > tc( cp, ssz, ssz + 1 );
    max = ( sz = ssz ) + 1;
    cstr = tc;
  }
}
String::String( char c, size_t num ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){
  while( num-- )
    put( c );
}
String::String( istream& cp ) : sz( 0 ), max( 1 ), cstr( 1 ), readpos( 0 ){
  while( 1 ){
    char c = cp.get();
    if( cp.good() )
      put( c );
    else 
      break;
  } 
}
void String::take( char* cp, size_t sz ){
  cstr = Cowa< char >( cp, sz );
  sz = sz - 1;
  max = sz;
}
String::~String( void ) {
  assess( max, "Corrupted string detected in String::Imp::~Imp" );
} 
String& String::operator=( const String& cp ){
  sz = cp.sz;
  max = cp.max;
  cstr = cp.cstr; 
  return *this;
}


const char* String::ccptr( void ) const{
  // Const cast away
  if ( cstr[ sz ] != '\000' )
	    ( (String*)this )->cstr.set( sz, '\000' );
  return cstr; 
}
size_t String::size( void ) const{
  return sz;
}
void String::put( const char* cp, size_t len, size_t start ){
  cp += start;
  const char* e = cp + len;
  while( cp != e ) 
    put( *( cp++ ) );
}
void String::put( const String& str ){
  put( str.ccptr(), str.size(), 0 );
}
void String::put( const String& str, size_t len, size_t start ){
  put( str.ccptr(), len, start );
}
void String::erase( void ){ 
  cstr = Cowa< char >( 1 );
  max = 1;
  sz = 0;
} 
void String::substitute( char o, char n ){
  for( size_t i = 0; i < sz; i++ )
    if( cstr[ i ] == o )
      cstr.set( i, n );
} 
void String::urldecode( void ){
  bool r = true;
  size_t n = 0;
  String ni;
  while( true ){ 
    while( ( n < sz ) && ( cstr[ n ] != '%' ) ){
      char c = cstr[ n++ ]; 
      ni.put( ( c == '+' )? ' ' : c );
    }
    if( n == sz )
      break;
    
    if( ++n == sz ){
      r = false;
      break;
    }
    char d1 = toupper( cstr[ n ] );
    if( !isxdigit( d1 ) || ( ++n == sz ) ){
      r = false; 
      break;
    }
    char d2 = toupper( cstr[ n++ ] );
    if( !isxdigit( d2 ) ){
      r = false;
      break;
    }
    if( d1 > 64 )
      d1 -= 55;
    else
      d1 -= 48;
    if( d2 > 64 )
      d2 -= 55;
    else
      d2 -= 48;
    ni.put( d1 * 16 + d2 );
  }
  
  if( !r )
    throw string_badparse( "Malformed url string detected in String::urldecode" );
  else{
    *this = ni;
  }
}
void String::htmlize( void ){
  size_t n = 0;
  String ni;
  while( true ){
    while( ( n < sz ) && 
	   ( cstr[ n ] != '<' ) && ( cstr[ n ] != '>' ) &&
	   ( cstr[ n ] != '&' ) && ( cstr[ n ] != '"' ) )
      ni.put( cstr[ n++ ] );
    
    if( n == sz )
      break;
    
    if( cstr[ n ] == '<' )
      ni += "&lt;";
    else if( cstr[ n ] == '>' )
      ni += "&gt;";
    else if( cstr[ n ] == '&' )
      ni += "&amp;";
    else 
      ni += "&quot;";
    
    n++;
  }

  *this = ni;
}
void String::flatten( void ){
  String ts;
  size_t i = 0;
  while( std::isspace( cstr[ i ] ) && ( i < sz ) )
    i++;
  while( i < sz ){
    while( !std::isspace( cstr[ i ] ) && ( i < sz ) )
      ts.put( cstr[ i++ ] );
    if( i == sz )
      break;
    while( std::isspace( cstr[ i ] ) && ( i < sz ) )
      i++;
    if( i != sz )
      ts.put( ' ' );
  } 
  *this = ts;	 
}
void String::paragraph( size_t wid, size_t ind, size_t flind ){
  if( !wid ){
    erase();
    return;
  }
  flatten();

  String ts;
  size_t space = wid;
  if( flind >= wid ) 
    ts = String( ' ', ind ) + '\n';
  else if( flind ){ 
    ts = String( ' ', flind );
    space -= flind;
  }
  ts += String( ' ', ind );
  size_t pos = 0;
  size_t len = 0;
  bool newl = false;

  while( pos < sz ){
    len = 0;
    if( cstr[ pos ] == ' ' )
      pos++;
    while( ( cstr[ pos + len ] != ' ' ) && ( ( pos + ++len ) < sz ) )
      ;
    size_t start = pos;
    pos += len;

    if( len >= wid ){
      if( space != wid ){ 
	ts += "\n" + String( ' ', ind );
	space = wid;
      }
      newl = false;
      ts += String( *this, start, wid ) + '\n' + String( ' ', ind );
      
    } else if( len > space ){
      ts += '\n' + String( ' ', ind ) + String( *this, start, len ) + ' ';
      space = ( wid - len ) - 1; 
      newl = true;

    } else {
      ts += String( *this, start, len );
      space -= len;
      if( space ){
	ts += ' ';
	space--;
      }
      newl = true;
    }
  }   
  if( newl )
    ts += '\n';
  *this = ts;
} 
void String::regex( const String re, const String repl ){
  GRegex gre( re );
  gre.search( *this );
  size_t i = 0;
  const char *cp = repl.ccptr();
  erase();
  while( i < repl.size() ){
    if( ( cp[ i ] == '\\' ) ){
      if( ++i >= repl.size() )
	put( '\\' );

      else {
	char c = cp[ i++ ];
	if ( ( c >= '0' ) && ( c <= '9' ) ){
	  if( ( (size_t)( c - '0' ) <= gre.subexps() ) && gre.found() )
	    put( gre[ c - '0' ] );
	} else
	  // Handle c escapes.
	  switch( c ){
	  case 'a':
	    put( '\a' );
	    break;
	  case 'b':
	    put( '\b' );
	    break;
	  case 'f':
	    put( '\f' );
	    break;
	  case 'n':
	    put( '\n' );
	    break;
	  case 'r':
	    put( '\r' );
	    break;
	  case 't':
	    put( '\t' );
	    break;
	  case 'v':
	    put( '\v' );
	    break;
	  default:
	    put( c );
	  }
      }

    } else
      put( cp[ i++ ] );
  }
}


void String::upcase( void ){
  size_t i = sz; 
  while( i-- )
    cstr.set( i, toupper( (unsigned char)cstr[ i ] ) );
}
void String::downcase( void ){
  size_t i = sz;
  while( i-- )
    cstr.set( i, tolower( (unsigned char)cstr[ i ] ) );
}


String String::nextline( void ) const{
  String a;
  while( ( cstr[ readpos ] != '\n' ) 
	 && ( cstr[ readpos ] != '\r' ) 
	 && ( readpos < sz ) )
    a.put( cstr[ readpos++ ] );
  if( cstr[ readpos ] == '\n' ){
    readpos++;
    lastlb = lf;
  } else if( cstr[ readpos ] == '\r' ){
    if( ( ( readpos + 1 ) < sz ) && ( cstr[ readpos + 1 ] == '\n' ) ){
      readpos += 2;
      lastlb = crlf;
    } else {
      readpos++;
      lastlb = cr;
    }
  } else
    lastlb = none;
  return a;
}
void String::reset( void ) const{
  readpos = 0;
}
bool String::more( void ) const{
  if( readpos < sz ) 
    return true;
  return false;
}
String String::lastlinebreak( void ) const{
  switch( lastlb ){
  case lf:
    return "\n";
  case cr:
    return "\r";
  case crlf:
    return "\r\n";
  default:
    return "";
  }
}


bool String::isspace( void ) const{
  size_t i = sz;
  while( i-- )
    if( !std::isspace( cstr[ i ] ) )
      return false;
  return true;
} 
bool String::isalnum( void ) const{
  size_t i = sz;
  while( i-- )
    if( !std::isalnum( cstr[ i ] ) )
      return false;
  return true;
} 
bool String::contains( char c ) const{
  size_t i = sz;
  while( i-- )
    if( cstr[ i ] == c )
      return true;
  return false;
}
bool String::prefix( const String& str ) const{
  if( sz > str.sz )
    return false;
  size_t i = 0;
  while( ( cstr[ i ] == str.cstr[ i ] ) && ( i < sz ) )
    i++;
  if( i == sz )
    return true;
  return false;
}


String& String::operator+=( const String& ap ){
  size_t m = ap.sz; 
  for( size_t i = 0; i < m; i++ )
    put( ap.cstr[ i ] );
  return *this;
}
String& String::operator*=( size_t mul ){
  if( !mul ){
    erase();
    return *this;
  }
  if( mul == 1 )
    return *this;
  String r( *this );
  for( size_t i = 1; i < mul; i++ )
    *this += r;
  return *this;
}
const String String::operator+( const String& ap ) const{
  String r( *this );
  r += ap;
  return r;
}
const String String::operator*( size_t mul ) const{
  String r( *this );
  r *= mul;
  return r;
}

 
bool String::operator>( const String& arg ) const{
  if( strcmp( this->ccptr(), arg.ccptr() ) > 0 )
    return true;
  else 
    return false;
}
bool String::operator>=( const String& arg ) const{
  if( strcmp( this->ccptr(), arg.ccptr() ) >= 0 )
    return true;
  else  
    return false;
}
bool String::operator<( const String& arg ) const{
  if( strcmp( this->ccptr(), arg.ccptr() ) < 0 )
    return true;
  else  
    return false;
}
bool String::operator<=( const String& arg ) const{
  if( strcmp( this->ccptr(), arg.ccptr() ) <= 0 )
    return true;
  else  
    return false;
}
 

char String::operator[]( size_t ind ) const{
  return cstr[ ind ];
}
bool String::operator!( void ) const{
  return !( sz );
}
bool String::operator==( const String& arg ) const{
  if( sz != arg.sz )
    return false;
  size_t i = sz;
  while( i-- )
    if( cstr[ i ] != arg.cstr[ i ] )
      return false;
  return true;
} 
bool String::operator!=( const String& arg ) const{
  if( sz != arg.sz )
    return true;
  size_t i = sz;
  while( i-- )
    if( cstr[ i ] != arg.cstr[ i ] )
      return true;
  return false;
}
       

istream& operator>>( istream& is, String& s){
  char c = 0;
  if( is ){
    while( is && isspace( c = is.get() ) );
    is.putback( c );
  }

  while( is ){
    char c( is.get() );
    if( isspace( c ) ){
      is.putback( c );
      break;
    }
    s.put( c );
  }
 
  return is;
}
ostream& operator<<( ostream& os, const String& s ){
  // Must be done a char at a time to prevent embedded nulls from
  // choking it up
  for( size_t i = 0; i < s.sz; i++ )
    os << s.cstr[ i ];
  return os;
}
const String operator+( const char* cp, const String& str ){
  return String( cp ) + str;
}
const String operator+( const char c, const String& str ){
  return String( c ) + str;
}

