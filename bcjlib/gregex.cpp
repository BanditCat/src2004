//////////////////////////////////////////////////////
// GNU regex library wrapper source file.
// (c) Jon DuBois 2004
// 02/27/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
//////////////////////////////////////////////////////
#include "bcj/gregex.H"
using namespace std; 



/////////////////////////////////////////////////////
// GRegex.
/////////////////////////////////////////////////////
struct GRegex::Imp{

  static bool inited;
  
  String str;  
  bool valid;
  int lastm;
  re_pattern_buffer rpb;
  re_registers rer;

  Imp( const String& );
  ~Imp( void );

};
bool GRegex::Imp::inited = false;


inline GRegex::Imp::Imp( const String& rge ) : str(), valid( false ){
  
  if( !inited ){
    re_syntax_options = RE_SYNTAX_POSIX_EXTENDED;
    inited = true;
  }

  rpb.translate = 0;
  rpb.fastmap = 0;
  rpb.buffer = 0;
  rpb.allocated = 0;
  rpb.fastmap = new char[ 256 ];
  rpb.regs_allocated = REGS_UNALLOCATED;
  rpb.not_bol = 0;
  rpb.not_eol = 0;
  rpb.newline_anchor = 0;
 
  const char* msg = re_compile_pattern( rge.ccptr() , rge.size(), &rpb );
 
  if( msg )
    throw regex_error( msg );
  
}
inline GRegex::Imp::~Imp( void ){
  delete[] rpb.fastmap;
  rpb.fastmap = NULL;
  regfree( &rpb );
} 
  


GRegex::GRegex( const String& rge ) : imp( new Imp( rge ) ) {}
GRegex::~GRegex( void ) { delete imp; }


bool GRegex::search( const String& rge ){
  imp->str = rge;
 
  int s = re_search( &imp->rpb, imp->str.ccptr(), imp->str.size(), 0, imp->str.size( ), &imp->rer );

  if( s == -2 ){
    imp->valid = false;
    throw regex_error( "Internal regular expression parser error" );
  }
  if( s == -1 ){
    imp->valid = false;
    imp->lastm = imp->str.size() - 1;
    return false;
  }
   
  imp->valid = true;
  imp->lastm = imp->rer.end[ 0 ];
  return true;
}
bool GRegex::next( void ){
  if( !imp->valid )
    return false;
  
  int s = re_search( &imp->rpb, imp->str.ccptr(), imp->str.size(), imp->lastm, imp->str.size( ), &imp->rer );

  if( s == -2 ){
    imp->valid = false;
    throw regex_error( "Internal regular expression parser error" );
  }
  if( s == -1 ){
    imp->valid = false;
    imp->lastm = imp->str.size() - 1;
    return false;
  }
   
  imp->valid = true;
  imp->lastm = imp->rer.end[ 0 ];
  return true;
}


size_t GRegex::subexps( void ) const{
  return imp->rpb.re_nsub;
}
bool GRegex::found( void ) const{
  return imp->valid;
}


String GRegex::operator[]( size_t index ) const{
  assess( imp->valid && ( index <= imp->rpb.re_nsub ),
	  "Request for invalid index to GRegex::operator[]" );

  regoff_t s = imp->rer.start[ index ];
  regoff_t e = imp->rer.end[ index ];
  if( s == e )
    return String();

  return String( imp->str, s, e - s );
}
bool GRegex::operator!( void ) const{
  return !imp->valid;
}
