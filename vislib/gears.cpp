//////////////////////////////////////////////////////
// vis v2 gears source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <stdexcept>
#include "vis/vis.H"
#include "vis/resource.H"

using namespace std; 

class handler : public Handler{
public:
  handler( Engine& engine ) : 
			   done( false ),
			   spinning( false ),
			   theengine( engine ),
			   surf( 1, 1 ),
			   roomtex ( "data/texture.bmp" ), squarebm( "data/squarebm.bmp" ), 
			   torusbm( "data/torusbm.bmp" ), 
			   grid( 25, 25 ),
			   torus( 1, 0.5, 30, 15 ),
			   box( 2, 2, 2, false ),
			   room( 20, 20, 20, true ),
			   fog( Fog::none, Color( 0, 0, 1 ), 2, 16, 1 ){
    theengine.screen.wireframecolor = Color( 0, 0, 1, 1 ); 
    theengine.screen.shadowwireframecolor = Color( 0, 1, 0, 1 ); 
    
    grid.ripple( 0 );
    torus.compile();
    grid.compile();

    zero.transform.identity();
    zero.shadowdepth = 0;
    zero.specular = false;
    zero.texture = &roomtex;
    zero.bumpmap = &flatbump;
    zero.specularmap = NULL;
    zero.object = &room;

    box1.specular = 0;
    box1.shadowdepth = 40;
    box1.texture = &boxtex;
    box1.bumpmap = &squarebm;
    box1.specularmap = NULL;
    box1.object = &box;
    box2.specular = 8;
    box2.shadowdepth = 40;
    box2.texture = &boxtex;
    box2.bumpmap = &squarebm;
    box2.specularmap = NULL;
    box2.object = &box;
    
    grid1.specular = 0;
    grid1.shadowdepth = 40;
    grid1.texture = &gridtex;
    grid1.bumpmap = &squarebm;
    grid1.specularmap = NULL;
    grid1.object = &grid;
    grid2.specular = 8;
    grid2.shadowdepth = 40;
    grid2.texture = &gridtex;
    grid2.bumpmap = &squarebm;
    grid2.specularmap = NULL;
    grid2.object = &grid;

    torus1.specular = 0;
    torus1.shadowdepth = 40;
    torus1.texture = &torustex;
    torus1.bumpmap = &torusbm;
    torus1.specularmap = NULL;
    torus1.object = &torus;
    torus2.specular = 8;
    torus2.shadowdepth = 40;
    torus2.texture = &torustex;
    torus2.bumpmap = &torusbm;
    torus2.specularmap = NULL;
    torus2.object = &torus;
    
    surf.setpixel( Point( 0, 0 ), Pixel( 127, 127, 255 ) );
    boxtex.load( surf );
    surf.setpixel( Point( 0, 0 ), Pixel( 255, 127, 127 ) );
    gridtex.load( surf );
    surf.setpixel( Point( 0, 0 ), Pixel( 127, 255, 127 ) );
    torustex.load( surf );
    surf.setpixel( Point( 0, 0 ), Pixel( 127, 127, 255 ) );
    flatbump.load( surf );

    view.viewport = theengine.screen.view().viewport;
    view.position = Vector( 0, 0, 4 );
    view.clipnear = 0.1;
    theengine.screen.shadowcolor = Color( 0, 0, 0, 1 );
    theengine.screen.light = Vector( 0, 0, 8 );
  }


  void tick( fpnum ticks ){
#ifdef DBG
    cout << "Entering handler::tick" << endl;
#endif

    theengine.screen.clear( Color( 0, 0, 0 ) );
    
    Angle viewangle( viewy.value, viewx.value, 0 );
    Vector eyespacez( viewangle );
    Vector eyespacey( Angle( viewangle.x + 90, viewangle.y, viewangle.z ) );
    Vector eyespacex( eyespacez );
    eyespacex.cross( eyespacey );
    if( theengine.button( MouseEvent::leftbutton ) && !theengine.button( MouseEvent::rightbutton ) ){
      objy += ( theengine.mousedy() / 4 );
      objx += ( theengine.mousedx() / 4 );
    } else if( theengine.button( MouseEvent::rightbutton ) ){
      if( !theengine.button( MouseEvent::leftbutton ) ){
	eyespacex *= ( theengine.mousedx() / 200 );
	eyespacey *= ( theengine.mousedy() / 200 );
	view.position -= eyespacex; 
	view.position -= eyespacey;
      } else {
	eyespacez *= ( theengine.mousedy() / 200 );
	view.position -= eyespacez;
      }
    } else {
      viewy += ( theengine.mousedy() / 4 );
      viewx += ( theengine.mousedx() / 4 );
    }
    
    if( spinning )
      lightx += ticks / 100;
    theengine.screen.light = Vector( Angle( lighty.value, lightx.value, 0 ) );
    theengine.screen.light *= 8;
        
    view.rotation = viewangle;
    theengine.screen.view( view );

    box1.transform.identity();
    box1.transform.rotate( Angle( objy.value, objx.value, objz.value ) );
    box1.transform.translate( Vector( 0, 0, 0 ) );
    box2.transform.identity();
    box2.transform.rotate( Angle( objy.value, objx.value, objz.value ) );
    box2.transform.translate( Vector( 0, 4, -4 ) );
    grid1.transform.identity();
    grid1.transform.scale( Vector( 2, 2, 1.5 ) );
    grid1.transform.rotate( Angle( objy.value, objx.value, objz.value ) );
    grid1.transform.translate( Vector( -4, 0, 0 ) );
    grid2.transform.identity();
    grid2.transform.scale( Vector( 2, 2, 1.5 ) );
    grid2.transform.rotate( Angle( objy.value, objx.value, objz.value ) );
    grid2.transform.translate( Vector( -4, 4, -4 ) );
    torus1.transform.identity();
    torus1.transform.rotate( Angle( objy.value, objx.value, objz.value ) );
    torus1.transform.translate( Vector( 4, 0, 0 ) );
    torus2.transform.identity();
    torus2.transform.rotate( Angle( objy.value, objx.value, objz.value ) );
    torus2.transform.translate( Vector( 4, 4, -4 ) );

    theengine.screen.draw( box1 );
    theengine.screen.draw( box2 );
    theengine.screen.draw( grid1 );
    theengine.screen.draw( grid2 );
    theengine.screen.draw( torus1 );
    theengine.screen.draw( torus2 );
    theengine.screen.draw( zero );

    theengine.screen.update();
  }

  
  void mousehandler( const MouseEvent& mouse ){
    if( mouse.type == MouseEvent::doubleclicked ){
      if( mouse.button == MouseEvent::middlebutton )  
	cout << theengine.fps() << endl;
      else if( mouse.button == MouseEvent::leftbutton ){
	if( fog.type == Fog::linear ){
	  fog.type = Fog::none;
	  theengine.screen.fog( fog );
	} else{
	  fog.type = Fog::linear;
	  theengine.screen.fog( fog );
	}
      } else if( mouse.button == MouseEvent::rightbutton )
	spinning = spinning ? false : true;

    }
  }

  void keyhandler( const KeyEvent& key ){
    if( key.pressed == true ){
      if( ( key.symbol == VK_ESCAPE ) || ( key.symbol == 'Q' ) )
	done = true;
      else if( key.symbol == 'W' ){
	if( theengine.screen.wireframe == Display::none )
	  theengine.screen.wireframe = Display::normal;
	else if( theengine.screen.wireframe == Display::normal )
	  theengine.screen.wireframe = Display::shadows;
	else if( theengine.screen.wireframe == Display::shadows )
	  theengine.screen.wireframe = Display::both;
	else
	  theengine.screen.wireframe = Display::none;
      }
    }
  }

  bool done;
private:
  bool spinning;
  Spinner viewx, viewy, objx, objy, objz, lightx, lighty;  
  View view;
  Engine& theengine;
  RGBSurface surf;
  Texture roomtex, boxtex, gridtex, torustex, squarebm, torusbm, flatbump;
  Grid grid;
  Torus torus;
  Rectangloid box;
  Rectangloid room;
  Fog fog;
  Render zero, grid1, grid2, torus1, torus2, box1, box2;
};



int WINAPI WinMain( HINSTANCE, HINSTANCE, LPSTR, int ) try{

#ifdef DBG
  cout << "Entering winmain" << endl;
#endif
#ifdef MEM_TEST
  SetMemTestVerbosity( true );
  SetMemTestOstream( cout );
#endif

  Engine theengine( IDI_ICON );

  theengine.fpsframes( 100 );
  
  if( !theengine.init( 800, 600, 24, 4, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 32, 4, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 15, 4, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 16, 4, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 24, 1, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 32, 1, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 15, 1, true, "Vislib test" , false ) &&
      !theengine.init( 800, 600, 16, 1, true, "Vislib test" , false ) )
    throw vis_error( "Unable to create window, try running at 32 or 24 bits per pixel" );

  cout << "Rendering with " << theengine.screen.glrenderer() << " from " << theengine.screen.glvendor() << endl;
  cout << "Version is " << theengine.screen.glversion() << endl;
  cout << "Available extensions: " << theengine.screen.glextensions() << endl;
  cout << "Using Cg with vertex profile " << CGShader::vertexprofile();
  cout << " and fragment profile " << CGShader::fragmentprofile() << endl;
  cout << "Compiled main fragment shader with this output:" << endl << theengine.screen.cgftexturecompile() << endl;
  cout << "Compiled main vertex shader with this output:" << endl << theengine.screen.cgvtexturecompile() << endl;

  handler hndlr( theengine );
  theengine.sethandler( hndlr );
    
  while( !hndlr.done && theengine.tick() )
    ;

  return 0;



} catch( run_error err ) {
  char* errmsg = new char[ 32768 ];
  strcpy( errmsg, "An irrecoverable error has occured: \n" );
  if( strlen( errmsg ) + strlen( err.what() ) < 32767 )
    strcat( errmsg, err.what() );
  else
    strcat( errmsg, "<Message to big to display>" );
  cerr << errmsg << endl;
  inform( errmsg, "Error" );
  delete[] errmsg;
  exit( EXIT_FAILURE );
  
} catch( runtime_error err ) {
  char* errmsg = new char[ 32768 ];
  strcpy( errmsg, "An irrecoverable error has occured: \n" );
  if( strlen( errmsg ) + strlen( err.what() ) < 32767 )
    strcat( errmsg, err.what() );
  else
    strcat( errmsg, "<Message to big to display>" );
  cerr << errmsg << endl;
  inform( errmsg, "Error" );
  delete[] errmsg;
  exit( EXIT_FAILURE );
  
} catch( ... ) {
  cerr << "An unhandled exception has occured." << endl;
  exit( EXIT_FAILURE );
}

