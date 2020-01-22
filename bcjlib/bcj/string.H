////////////////////////////////////////////////
// String header
// (c) Jon DuBois 2004
// 02/28/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef STRING_H
#define STRING_H
#include <cstddef>
#include <iostream>
#include "bcj/cowa.H"
#include "bcj/assess.H"


// Amount to multiply string by when out of space
const static size_t granularity = 2;



/////////////////////////////////////////////
// Exceptions
/////////////////////////////////////////////
struct string_error : public run_error{
  string_error( const char* msg = "Unspecified string error" ) : run_error( msg ) {}
};
struct string_badparse : public string_error{
  string_badparse( const char* msg = "Error while parsing a string" ) : string_error( msg ) {}
};



/////////////////////////////////////////////
// String
/////////////////////////////////////////////
class String{ 

public:

  String( void );
  String( const String& );
  // Substring copy constructor.
  String( const String&, size_t start, size_t length );
  String( const char* );
  // For non-NULL terminated strings of known length
  String( const char*, size_t );
  // Creates a string of the same character of length n.
  String( char, size_t = 1 );
  String( std::istream& );
  String( int );
  String( size_t );
  ~String( void );
  String& operator=( const String& ); 
  // This takes ownership of a char array, including deleting it 
  // when finished, the size given must include the terminating null.
  void take( char*, size_t sz );

  const char* ccptr( void ) const;
  size_t size( void ) const;
  void put( char c );
  void put( const char*, size_t len, size_t sind = 0 );
  void put( const String& );
  void put( const String&, size_t len, size_t sind = 0 );
  void erase( void );
  void substitute( char oldc, char newc );
  void upcase( void );
  void downcase( void );

  // Returns the next line of s, or nothing
  String nextline( void ) const;
  // This resets the postion
  void reset( void ) const;
  // This is true iff there is more to be read
  bool more( void ) const;
  // This returns the last linebreak( eg. \n, \r, or \r\n ).
  String lastlinebreak( void ) const; 

  // This %-decodes the string, i.e. for CGI
  void urldecode( void );
  // This turns <>&" into html entities, i.e. for embedding in html 
  void htmlize( void );
  // This compresses all whitespace down to a single space
  void flatten( void );
  // This turns a string into a paragraph of width wid, indenting the first
  // line flind + ind, and all other lines ind
  void paragraph( size_t wid, size_t ind = 0, size_t flind = 0 );
  // This does regex replacment on the string, with backrefrences and
  // std c escapes.
  void regex( const String regexp, const String repl );

  bool isspace( void ) const;
  bool isalnum( void ) const;
  bool contains( char c ) const;
  // True if the string is a prefix of str
  bool prefix( const String& str ) const;

  String& operator+=( const String& );
  String& operator*=( size_t mul );
  const String operator+( const String& ) const;
  const String operator*( size_t mul ) const;

  bool operator>( const String& ) const;
  bool operator>=( const String& ) const;
  bool operator<( const String& ) const;
  bool operator<=( const String& ) const;

  char operator[]( size_t ) const;
  bool operator!( void ) const;
  bool operator==( const String& ) const;
  bool operator!=( const String& ) const;
  // Appends the next contiugously non-whitespace region of an istream onto 
  // the back of a String.
  friend std::istream& operator>>( std::istream&, String& );
  friend std::ostream& operator<<( std::ostream&, const String& );
  friend const String operator+( const char*, const String& );
  friend const String operator+( const char, const String& );

private:

  // The last line terminator
  mutable enum { lf, crlf, cr, none } lastlb;
  // Size NOT including null, e.g. a zero-length string has size 0.
  size_t sz;
  // Size of buffer.
  size_t max;
  // This is mutable, I prefer const cast away over the mutable keyword
  Cowa< char > cstr;
  // Read position within string.
  mutable size_t readpos;
  
};



///////////////////////////////////////////////////////////////////////////
// Necessary evil.
///////////////////////////////////////////////////////////////////////////
inline void String::put( char c ){
  if( ( sz + 1 ) >=  max ){
    Cowa< char > tb( cstr, sz, max * granularity );
    max *= granularity;
    cstr = tb;
  }
  cstr.set( sz++, c );
}



#endif //STRING_H


