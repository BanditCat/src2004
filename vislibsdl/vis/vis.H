////////////////////////////////////////////////
// Visualization v2 header.
// (c) Jon DuBois 2004
// 10/22/2004
// This file is licensed via the GPL.
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef VIS_H
#define VIS_H
#include <cmath>
#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>


////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////
typedef Uint8 byte;
typedef Uint16 word;
typedef Uint32 dword;
typedef Sint8 sbyte;
typedef Sint16 sword;
typedef Sint32 sdword;
typedef GLfloat fpnum;
typedef GLdouble dfpnum;
static const fpnum pi = 3.141592653589793238462643383279502884197169399375105820974944592308; 
static const fpnum torad = ( pi * 2 / 360 );


////////////////////////////////////////////////
// Exceptions
////////////////////////////////////////////////
struct vis_error : public run_error {
  vis_error( const char* msg = "Undefined error in libvis" ) : run_error( msg ) {}
};
struct vis_environment_error : public vis_error {
  vis_environment_error( const char* msg = 
			 "Undefined environment error in libvis" ) : vis_error( msg ) {}
};



////////////////////////////////////////////////
// Spinners
// These provide a nice little rotatable value
////////////////////////////////////////////////
struct Spinner{
  Spinner( void ) : value( 0 ){}
  Spinner( fpnum rot ) : value( rot ){}
  ~Spinner( void ){}

  fpnum cos( void ){ return cosf( value ); }
  fpnum sin( void ){ return sinf( value ); }
  Spinner& operator+=( fpnum d ){
    value += d;
    while( value > 360 )
      value -= 360;
    while( value < 0 )
      value += 360;
    return *this;
  }
  Spinner& operator-=( fpnum d ){
    value -= d;
    while( value > 360 )
      value -= 360;
    while( value < 0 )
      value += 360;
    return *this;
  }

  fpnum value;
};



///////////////////////////////////////////////
// Handlers
///////////////////////////////////////////////
struct MouseEvent{
  bool pressed;
  byte button;
};
struct KeyEvent{
  bool pressed;
  byte scancode;
  dword symbol;
};
class Handler{
public:
  virtual ~Handler( void ){}
  virtual void mousehandler( const MouseEvent& ) = 0;
  virtual void keyhandler( const KeyEvent& ) = 0;
  virtual void tick( fpnum tcks ) = 0;
};


#include "vis/surface.H"
#include "vis/geometry.H"
#include "vis/draw.H"
#include "vis/lighting.H"
#include "vis/context.H"
#include "vis/engine.H"


#endif //VIS_H
