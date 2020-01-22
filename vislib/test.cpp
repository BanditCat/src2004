//////////////////////////////////////////////////////
// vis v2 test source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include "vis/vis.H"



using namespace std; 
 


void fail( void ){
  inform( "Test failed!", "Error" );
  exit( EXIT_FAILURE );
}


int main( int argc, char **argv ) try{

#ifdef MEM_TEST
  SetMemTestVerbosity( true );
#endif

  srand( time( NULL ) );
  inform( "Testing stacks and algorithims", "Test" );

  {
    ObjStack< int > l;  
    bool failed = false;
    static const size_t size = 10; 
    for( size_t j = 0; j < 100; j++ ){ 
      
      for( size_t i = 0; i < size; i++ )
	l.push( rand() % 100 );
      
      Sort( l );
 
      {
	ObjStack< int >::Iterator ib( l.start() );
	ObjStack< int >::Iterator ie( l.end() );
	do{ 
	  ObjStack< int >::Iterator t( ib++ );
	  if( *t > *ib )
	    failed = true; 
	}while( ib != ie );
      }

      l.empty();
    }
    
    if( failed ) fail();
  }
 
  {
    Stack< int > l;  
    bool failed = false;
    static const size_t size = 10; 
    for( size_t j = 0; j < 100; j++ ){ 
      
      for( size_t i = 0; i < size; i++ )
	l.push( rand() % 100 );
      
      Sort( l );
 
      {
	Stack< int >::Iterator ib( l.start() );
	Stack< int >::Iterator ie( l.end() );
	do{ 
	  Stack< int >::Iterator t( ib++ );
	  if( *t > *ib )
	    failed = true; 
	}while( ib != ie );
      }

      l.empty();
    }
    
    if( failed ) fail();
  }
 
  inform( "Success!", "Test succeeded" );
  inform( "Testing matrices and vectors", "Test" );

  {
    Matrix m, mi;
    m.identity();
    mi.identity();

    Vector v( ( (fpnum)( rand() % 10000 ) / 100.0f ) - 50.0f, 
	      ( (fpnum)( rand() % 10000 ) / 100.0f ) - 50.0f, 
	      ( (fpnum)( rand() % 10000 ) / 100.0f ) - 50.0f );

    Vector u( v );

    {
      Vector w( ( (fpnum)( rand() % 10000 + 1 ) / 100.0f ), 
		( (fpnum)( rand() % 10000 + 1 ) / 100.0f ), 
		( (fpnum)( rand() % 10000 + 1 ) / 100.0f ) );
      m.scale( w );
      Matrix n;
      n.identity();
      w.x = 1 / w.x;
      w.y = 1 / w.y;
      w.z = 1 / w.z;
      n.scale( w );
      mi.imultiply( n );
    }
    
    v.transform( m );
    m.invert();
    v.transform( m );
    m.invert();

    u.transform( m );
    u.transform( mi );

    {
      Angle a( rand() % 360, rand() % 360, rand() % 360 );
      m.rotate( a );
      Matrix n;
      n.identity();
      n.unrotate( a );
      mi.imultiply( n );
    }
    
    u.transform( m );
    m.invert();
    u.transform( m );
    m.invert();

    v.transform( m );
    v.transform( mi );

    {
      Vector w( ( (fpnum)( rand() % 10000 ) / 100.0f - 50.0f ), 
		( (fpnum)( rand() % 10000 ) / 100.0f - 50.0f ), 
		( (fpnum)( rand() % 10000 ) / 100.0f - 50.0f ) );
      w.normal();
      fpnum a = ( (fpnum)( rand() % 10000 ) / 100.0f ) - 50.0f;
      m.rotate( w, a );
      Matrix n;
      n.identity();
      n.rotate( w, -a );
      mi.imultiply( n );
    }
    
    v.transform( m );
    m.invert();
    v.transform( m );
    m.invert();

    u.transform( m );
    u.transform( mi );
    
    {
      Vector w( ( (fpnum)( rand() % 10000 ) / 100.0f - 50.0f ), 
		( (fpnum)( rand() % 10000 ) / 100.0f - 50.0f ), 
		( (fpnum)( rand() % 10000 ) / 100.0f - 50.0f ) );
      m.translate( w );
      Matrix n;
      n.identity();
      w *= -1;
      n.translate( w );
      mi.imultiply( n );
    }
    
    v.transform( m );
    m.invert();
    v.transform( m );
    m.invert();

    u.transform( m );
    u.transform( mi );

    u.transform( mi );
    mi.invert();
    u.transform( mi );


    u -= v;

    if( u.length() > 0.1 )
      fail();
  }

  inform( "Success!", "Test succeeded" );

} catch( ... ) {
  inform( "Test failed! An exception has occured", "Error" );
}
