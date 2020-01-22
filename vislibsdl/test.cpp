//////////////////////////////////////////////////////
// vis v2 test source file.
// (c) Jon DuBois 2004
// 10/22/2004
// This file is licensed via the GPL.
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <stdexcept>
#include <ctime>
#include <GL/gl.h>	       
#include <GL/glu.h>	       
#include <SDL/SDL.h>	       
#include "vis/assess.H"
#include "vis/vis.H"

using namespace std; 

Engine theengine( 640, 480, "Perpixel lighting" );

class handler : public Handler{
public:
  handler( void ) : done( false ),
		    obj( 35, 35 ), box( 16, 16, 16, true ){
    theengine.screen.anisotropic = false;
    view.viewport = theengine.screen.view().viewport;
    view.position = Vector( 0, 0, 4 );
    view.clipnear = 0.1;
    //bumpmap.makebumpmap( 0.0125 );
    // theengine.screen.texture( texture ); 
    //    engine().screen.specularmap( specular );
    obj.shadowdepth = 40;
    obj.ripple( 0 );
    obj.specular = true;
    //theengine.screen.bumpmap( bumpmap ); 
    //    engine().screen.emissionmap( emission );
    theengine.screen.shadowcolor = Color( 0, 0, 0, 0.25 );
    theengine.screen.light = Vector( 0, 0, 8 );
  }


  void tick( fpnum ticks ){
    Angle viewangle( viewy.value, viewx.value, 0 );
    Vector eyespacez( viewangle );
    Vector eyespacey( Angle( viewangle.x + 90, viewangle.y, viewangle.z ) );
    Vector eyespacex( eyespacez );
    eyespacex.cross( eyespacey );
    if( theengine.buttons() == 1 ){
      objy += ( theengine.mousedy() / 4 );
      objx += ( theengine.mousedx() / 4 );
    } else if( theengine.buttons() == 4 ){
      eyespacex *= ( theengine.mousedx() / 200 );
      eyespacey *= ( theengine.mousedy() / 200 );
      view.position -= eyespacex; 
      view.position -= eyespacey;
    } else if( theengine.buttons() == 5 ){
      eyespacez *= ( theengine.mousedy() / 200 );
      view.position -= eyespacez;
    } else {
      viewy += ( theengine.mousedy() / 4 );
      viewx += ( theengine.mousedx() / 4 );
    }
    
    view.rotation = viewangle;
    obj.rotation = Angle( objy.value, objx.value, objz.value );
    theengine.screen.light = Vector( Angle( lighty.value, lightx.value, 0 ) );
    theengine.screen.light *= 8;
    theengine.screen.view( view );
    obj.position = Vector( 0, 0, 0 );
    theengine.screen.draw( obj );
    obj.position = Vector( 2, 0, -5 );
    theengine.screen.draw( obj );
    theengine.screen.draw( box );
  }


  void mousehandler( const MouseEvent& mouse ){
    if( mouse.pressed ){
      if( mouse.button == SDL_BUTTON_MIDDLE ) 
	cout << theengine.fps() << endl;
    }
  }
  void keyhandler( const KeyEvent& key ){
    if( key.symbol == SDLK_ESCAPE )
      done = true;
    if( key.symbol == SDLK_F1 )
      cout << theengine.fps() << endl;
  }

  bool done;
private:
  bool spinning;
  Spinner viewx, viewy, objx, objy, objz, lightx, lighty;  
  View view;
  //RGBSurface texture, bumpmap;
  Grid obj;
  Rectangloid box;
};


int main( int argc, char **argv ) try{
#ifdef MEM_TEST
  SetMemTestVerbosity( true );
#endif
  
  handler hndlr;
  theengine.sethandler( hndlr );
    
  while( !hndlr.done ){
    
    theengine.tick();
      
  }

  return 0;
   
} catch( run_error err ) {
  cerr << "A programming error has manifested itself:"
       << endl << err.what() << endl;
  exit( EXIT_FAILURE );
} catch( runtime_error err ) {
  cerr << "An irrecoverable error has occured:"
       << endl << err.what() << endl;
  exit( EXIT_FAILURE );
} catch( ... ) {
  cerr << "An unhandled exception has occured." << endl;
  exit( EXIT_FAILURE );
}
