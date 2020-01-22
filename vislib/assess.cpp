//////////////////////////////////////////////////////
// Debugging source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
//////////////////////////////////////////////////////
#include <iostream>
#include "vis/assess.H"


using namespace std; 



//////////////////////////////////////////////////////
// Utility
//////////////////////////////////////////////////////
bool question( const char* message, const char* title ){
  ShowCursor( true );
  if( MessageBox( NULL, message, title, MB_YESNO | MB_ICONQUESTION ) == IDNO )
    return false;
  else 
    return true;
  ShowCursor( false );
}
void inform( const char* message, const char* title ){ 
  ShowCursor( true );
  MessageBox( NULL, message, title, MB_OK | MB_ICONERROR );
  ShowCursor( false );
}

/////////////////////////////////////////////////////
// Memory tracking.
/////////////////////////////////////////////////////
#ifdef MEM_TEST
size_t NumAllocs;
ostream* MemTestOstream = &cerr;
bool MemTestVerbose = false;



void SetMemTestOstream( ostream& os ){
  MemTestOstream = &os;
}


void SetMemTestVerbosity( bool v ){
  MemTestVerbose = v;
}


size_t GetAllocCount( void ){
  return NumAllocs;
}


void* operator new( size_t sz ) throw( bad_alloc ){
  void* vp = malloc( sz );
  ++NumAllocs;
  if( !vp )
    throw bad_alloc();
  if( MemTestVerbose && MemTestOstream->good() )
     *MemTestOstream << "Allocating " << vp << ", there are now " << NumAllocs << " allocations." << endl;
  return vp;
}


void operator delete( void* vp ) throw(){
  --NumAllocs;
  if( !vp )
    throw bad_free();
  free( vp );
  if( MemTestVerbose && MemTestOstream->good() )
    *MemTestOstream << "Deallocating " << vp << ", there are now " << NumAllocs << " allocations." << endl;
}



#endif //MEM_TEST
