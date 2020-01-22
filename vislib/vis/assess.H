////////////////////////////////////////////////
// Debuging header.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef ASSESS_H
#define ASSESS_H
#include <iostream>
#include <windows.h>



////////////////////////////////////////////////
// Exceptions.
////////////////////////////////////////////////
class run_error{
  const char* emsg;
public:
  run_error( const char* msg = "Unspecified runtime error" ) : emsg( msg ) {}
  const char* what( void ) const { return emsg; }
};
struct failed_assessment : public run_error{
  failed_assessment( const char* msg = "Failed assessment" ) : run_error( msg ) {} 
};
struct bad_free : public run_error {
  bad_free( const char* msg = "Attempt to delete bad pointer" ) : run_error( msg ) {}
};



///////////////////////////////////////////////
// Functions.
///////////////////////////////////////////////
#ifndef NO_ASSESS
inline void assess( bool r, const char* msg ){
  if( !r ) {
    throw failed_assessment( msg );
  }
}
#else
inline void assess( bool, const char* ){}
#endif //NO_ASSESS

// Utility functions, question returns true if the user clicked yes and false otherwise.
bool question( const char* message, const char* title );
void inform( const char* message, const char* title );

#ifdef MEM_TEST 
void SetMemTestOstream( std::ostream& );
void SetMemTestVerbosity( bool );
size_t GetAllocCount( void );
#endif //MEM_TEST



#endif //ASSESS_H
