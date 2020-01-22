//////////////////////////////////////////////////////
// Debugging source file.
// (c) Jon DuBois 2004
// 02/27/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
//////////////////////////////////////////////////////
#include <iostream>
#include "bcj/assess.H"
using namespace std; 



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
