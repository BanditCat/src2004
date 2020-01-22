//////////////////////////////////////////////////////
// vis engine source file.
// (c) Jon DuBois 2004
// 05/04/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <SDL/SDL.h>
#include "bcj/assess.H"
#include "bcj/string.H"
#include "vis/vis.H"

using namespace std; 



/////////////////////////////////////////////////////
// Engine.
///////////////////////////////////////////////////// 
// class Engine{
// public:
//   Engine( word, word, const char* );
//   ~Engine( void );

//   // The main function;
//   void tick( void );
//   void sethandler( Handler& );

//   // Mouse
//   fpnum mousedx( void ) const{ return mdx; }
//   fpnum mousedy( void ) const{ return mdy; }
//   byte buttons( void ) const{ return bs; }
  
//   // Info
//   fpnum fps( void ) const{ return afps; }

//   Display screen;

// private:
//   // These are used to calculate the average fps over the last 20 frames.
//   fpnum afps;
//   dword atcks[ 20 ];
//   byte apos;

//   bool fcs;
//   byte bs;
//   float mdx, mdy;
//   size_t bpp;
//   static bool& exists( void ){ static bool pexists = false; return pexists; }
//   Handler* hndlr;
//   // The number of ticks last time tick was called.
//   dword ltcks;
// };
Engine::Engine( word w, word h, const char* nm) : screen( w, h, nm ),
						  afps( 0 ), apos( 0 ), fcs( true ),
						  bs( 0 ), mdx( 0 ), mdy( 0 ), hndlr( NULL ){
  if( exists() )
    throw vis_error( "Attempt to double instatiate the engine" );
  exists() = true;
}
Engine::~Engine( void ){
  exists() = false;
}


void Engine::tick( void ){
  // get timer count;
  dword tcks = SDL_GetTicks();
  if( tcks == ltcks ){
    SDL_Delay( 1 );
    return;
  }
  
  if( !hndlr )
    throw vis_error( "Attempt to use Engine::tick() without a handler installed" );
  screen.clear( Color( 0, 0, 0 ) );
  SDL_Event evt;
  // zero mouse deltas;
  mdx = 0;
  mdy = 0;
  word mx = screen.width() / 2;
  word my = screen.height() / 2;
  while( SDL_PollEvent( &evt ) ){
    switch( evt.type ) {
    case SDL_KEYDOWN:
    case SDL_KEYUP:
      KeyEvent ke;
      if( evt.type == SDL_KEYUP )
	ke.pressed = false;
      else
	ke.pressed = true;
      ke.scancode = evt.key.keysym.scancode;
      ke.symbol = evt.key.keysym.sym;
      hndlr->keyhandler( ke ); 
      break;
      
    case SDL_MOUSEBUTTONUP:
    case SDL_MOUSEBUTTONDOWN:
      MouseEvent me;
      if( evt.type == SDL_MOUSEBUTTONUP )
	me.pressed = false;
      else
	me.pressed = true;
      me.button = evt.button.button;
      hndlr->mousehandler( me );
      break;
      
    case SDL_MOUSEMOTION:
      if( fcs ){
	mdx = (fpnum)mx - (fpnum)evt.motion.x;
	mdy = (fpnum)my - (fpnum)evt.motion.y;
	bs = evt.motion.state;
      }
      break;

    case SDL_QUIT:
      exit( 0 );

    case SDL_ACTIVEEVENT:
      if( evt.active.state == SDL_APPINPUTFOCUS )
	if( evt.active.gain )
	  fcs = true;
	else
	  fcs = false;
      break;
    }
  }
  if( fcs )
    SDL_WarpMouse( mx, my );
  if( tcks < ltcks )
    hndlr->tick( 0 );
  else
    hndlr->tick( tcks - ltcks );
  
  if( atcks[ apos ] )
    afps -= 50 / (fpnum)atcks[ apos ];
  atcks[ apos ] = tcks - ltcks;
  afps += 50 / (fpnum)atcks[ apos ];
  if( ++apos == 20 )
    apos = 0;

  ltcks = tcks;
  screen.update();
}
void Engine::sethandler( Handler& hnd ){
  hndlr = &hnd;
}
