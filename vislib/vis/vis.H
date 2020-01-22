////////////////////////////////////////////////
// Visualization v2 header.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef VIS_H
#define VIS_H
#include <cmath>
#include <iostream>
#include "vis/assess.h"


////////////////////////////////////////////////
// typedefs
////////////////////////////////////////////////
typedef unsigned char byte;
typedef unsigned short int word;
typedef unsigned int dword;
typedef signed char sbyte;
typedef signed short int sword;
typedef signed int sdword;
typedef float fpnum;
typedef double dfpnum;
static const fpnum pi = 3.141592653589793238462643383279502884197169399375105820974944592308; 
static const fpnum torad = ( pi * 2 / 360 );



////////////////////////////////////////////////
// Exceptions
////////////////////////////////////////////////
struct vis_error : public run_error {
  vis_error( const char* msg = "Undefined error in libvis" ) : run_error( msg ) {}
};
struct vis_environment_error : public vis_error {
  vis_environment_error( const char* msg = "Undefined environment error in libvis" ) : vis_error( msg ) {}
};
struct vis_bad_load : public vis_error {
  vis_bad_load( const char* msg = "Failed to load" ) : vis_error( msg ) {}
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


#include "vis/algo.H"
#include "vis/stack.H"
#include "vis/surface.H"
#include "vis/geometry.H"
#include "vis/draw.H"
#include "vis/lighting.H"
#include "vis/display.H"
#include "vis/engine.H"
#include "vis/cgshader.H"



#endif //VIS_H
