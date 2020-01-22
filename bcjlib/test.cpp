//////////////////////////////////////////////////////
// Cgi binary for sending messages.
// (c) Jon DuBois 2004
// 03/3/2004
// See license.txt for license details.
//////////////////////////////////////////////////////
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include "bcj/bcjlib.H"
using namespace std; 
 


void fail( void ){
  cout << "Test failed!" << endl;
  exit( EXIT_FAILURE );
}


int main( int argc, char **argv ) try{

#ifdef MEM_TEST
  SetMemTestVerbosity( true );
#endif
  
  srand( time( NULL ) );


  {
    cout << "Testing Strings... ";
    String t( "This is a test." );

    bool failed = true;
    try{
      String t( (const char*)NULL );
    } catch( string_error ) {
      failed = false;
    }
    if( failed ) fail();
    try{
      String t( (const char*)NULL );
    } catch( string_error ) {
      failed = false;
    }
    if( failed ) fail();
     
    String a( t, 5, 2 );
    a *= 2;
    if( a != "isis" ) fail();
    t += a + ( String( "foo" ) * 2 );
    if( t != "This is a test.isisfoofoo" ) fail();
    a *= 0;
    t += a;
    a += t;
    if( t != a ) fail();
    String s( a.ccptr(), 4 );
    if( s != "This" ) fail();
    a = String( 'a', 0 );
    if( !( !a ) ) fail();
    a = String( 'a', 1 );
    t = String( -12 );
    if( ( a + t ) != "a-12" ) fail();
    size_t i = 65535;
    if( ( s = ( String( i ) + s ) ) != "65535This" ) fail();
    s.substitute( '6', 'i' );
    s.substitute( '5', 's' );
    s.substitute( 's', 'n' );
    if( s != "inn3nThin" ) fail();
    s.upcase();
    if( s != "INN3NTHIN" ) fail();
    s.downcase();
    if( s != "inn3nthin" ) fail();
    t = "%3cv%26a%3E%3b";
    t.urldecode();
    if( t != "<v&a>;" ) fail();
    failed = true;
    t = "%3g%26a%3E%3b";
    try{
      t.urldecode();
    } catch( string_badparse ) {
      failed = false;
    }
    if( failed ) fail();
    failed = true;
    t = "%3cv%26a%3E%3"; 
    try{
      t.urldecode();
    } catch( string_badparse ) {
      failed = false;
    }
    if( failed ) fail();
    t += 'b';
    t += '"';
    t.urldecode();
    t.htmlize();
    if( t != "&lt;v&amp;a&gt;;&quot;" ) fail();
    t = " \t\n";
    if( !t.isspace() ) fail();
    t = " foo\t";
    if( t.isspace() ) fail();
    t = "";
    if( !t.isspace() ) fail();
    if( !t.isalnum() ) fail();
    t = "(*& aa !@#";
    if( t.isalnum() ) fail();
    t = "abcdefghijklmnopqrstuvwxyz1234567890";
    if( !t.isalnum() ) fail();
    if( !t.contains( 'a' ) ) fail();
    if( !t.contains( '0' ) ) fail();
    if( !t.contains( 'p' ) ) fail();
    if( t.contains( ' ' ) ) fail();
    t = "foo";
    if( ( "bar" + t ) != "barfoo" ) fail();
    a = 'a';
    t = "bc";
    s = "c";
    if( ( a > t ) ) fail();
    if( !( s > t ) ) fail();
    if( ( s < t ) ) fail();
    if( !( a < s ) ) fail();
    if( ( a >= t ) ) fail();
    if( !( t >= "bc" ) ) fail();
    if( ( s <= a ) ) fail();
    if( !( s <= 'c' ) ) fail();
    a = "\n\tThis  is\na\ttest.";
    a.flatten();
    if( a != "This is a test." ) fail();
    a = "\n\ta  b";
    a.flatten();
    if( a != "a b" ) fail();
    a = "a  b  \t";
    a.flatten();
    if( a != "a b" ) fail();
    a = "\n\ta\tb\n\t";
    a.flatten();
    if( a != "a b" ) fail();
    a = "This is a test.";
    a.paragraph( 6 );
    if( a != "This \nis a \ntest. \n" ) fail();
    t = a;
    a.paragraph( 2, 2, 1 );
    if( a != "   \n  Th\n  is\n  a \n  te\n  " ) fail();
    t.paragraph( 5, 3, 1 );
    if( t != "    This\n   is a \n   test.\n   " ) fail(); 
    a = "This is a test.";
    a.paragraph( 1, 3, 2 );
    if( a != "   \n   T\n   i\n   a\n   t\n   " ) fail();
    a.paragraph( 0, 3, 2 );
    if( !!a ) fail();
    
    a = "This (is) (a) (test) of /reg/exps/with/back/refs";
    a.regex( "(\\([^)]*\\) *)(\\([^)]*\\) *)(\\([^)]*\\) *) *of *((/[^\\/]*)+)$", "\\1\\v\\2\\b\\3\\t\\4\\n" );
    if( a != "(is) \v(a) \b(test) \t/reg/exps/with/back/refs\n" ) fail();

    a = "a long\nyoda\nlike saying\nthis is.";
    if( a.nextline() != "a long" ) fail();
    if( a.nextline() != "yoda" ) fail();
    if( a.nextline() != "like saying" ) fail();
    a.reset();
    if( a.nextline() != "a long" ) fail();
    if( a.nextline() != "yoda" ) fail();
    if( a.nextline() != "like saying" ) fail();
    if( a.nextline() != "this is." ) fail();

  }

  cout << "Success!" << endl;

  cout << "Testing Containers and algorithims... ";
 
  {
    Stack< int > l;  
    bool failed = false;
    static const size_t size = 10; 
    for( size_t j = 0; j < 100; j++ ){ 
      
      for( size_t i = 0; i < size; i++ )
	l.push( rand() % 100 );
      
      Sort( l.start(), l.end() );
 
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

  {
    List< String > ls;
    ls.push( "long " );
    List< String >::Iterator b( ls.start() );
    List< String >::Iterator e( ls.end() );
    
    if( e != b ) fail();
    
    ls.insert( b, "is " );
    ls.pushback( "a " );
    ls.pushback();
    b -= 2;
    *b += "Foobar";

    {
      List< String >::Iterator tb( ls.start() );
      if( *tb++ != "Foobar" ) fail();
      if( *tb++ != "a " ) fail();
      if( *tb++ != "is " ) fail();
      if( *tb++ != "long " ) fail();
    }


    ls.remove( b );
    ls.insert( e, "windy " );
    b++;
    ls.insert( b );
    *b += "this ";

    {
      List< String >::Iterator tb( ls.start() );
      if( *tb++ != "a " ) fail();    
      if( *tb++ != "this " ) fail(); //b
      if( *tb++ != "is " ) fail();   
      if( *tb++ != "windy ") fail(); //e
      if( *tb++ != "long " ) fail();
    }

    ls.push();
    ls.peek() += "sentence ";
    e++; b++;
    {
      List< String >::Iterator tb( ls.start() );
      if( *tb++ != "a " ) fail();    
      if( *tb++ != "this " ) fail(); 
      if( *tb++ != "is " ) fail();   //b
      if( *tb++ != "windy ") fail();  
      if( *tb++ != "long " ) fail(); //e
      if( *tb++ != "sentence " ) fail();
    }
    e -= 3;
    b.swap( e-- );
    e.swap( b++ );
    {
      List< String >::Iterator tb( ls.start() );
      if( *tb++ != "this " ) fail(); //e    
      if( *tb++ != "is " ) fail(); 
      if( *tb++ != "a " ) fail();   
      if( *tb++ != "windy ") fail(); //b  
      if( *tb++ != "long " ) fail();
      if( *tb++ != "sentence " ) fail();
    }
    e += 4;
    b.swap( e++ );
    b -= 3;
    {
      List< String >::Iterator tb( ls.start() );
      if( *tb++ != "this " ) fail(); //b
      if( *tb++ != "is " ) fail(); 
      if( *tb++ != "a " ) fail();   
      if( *tb++ != "long ") fail();   
      if( *tb++ != "windy " ) fail(); 
      if( *tb++ != "sentence " ) fail(); //e
    }
  }
  
  {
    List< int > l;  
    bool failed = false;
    static const size_t size = 10; 
    for( size_t j = 0; j < 100; j++ ){ 
      
      for( size_t i = 0; i < size; i++ )
	l.push( rand() % 100 );
      
      Sort( l.start(), l.end() );
 
      {
	List< int >::Iterator ib( l.start() );
	List< int >::Iterator ie( l.end() );
	do{ 
	  List< int >::Iterator t( ib++ );
	  if( *t > *ib )
	    failed = true; 
	}while( ib != ie );
      }

      l.empty();
    }
    
    if( failed ) fail();
  }

 
  cout << "Success!" << endl;

} catch( run_error err ) {
  cerr << "Test failed! An exception has occured:"
       << endl << err.what() << endl;
  exit( EXIT_FAILURE );
} catch( runtime_error err ) {
  cerr << "Test failed! An exception has occured:"
       << endl << err.what() << endl;
  exit( EXIT_FAILURE );
} catch( ... ) {
  cerr << "Test failed! An exception has occured." << endl;
  exit( EXIT_FAILURE );
}
