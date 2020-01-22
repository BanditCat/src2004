////////////////////////////////////////////////
// Visualization v2 lighting header.
// (c) Jon DuBois 2004
// 10/23/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef LIGHTING_H
#define LIGHTING_H
#include <GL/gl.h>
#include "vis/vis.H"


/////////////////////////////////////////////
// Colors
/////////////////////////////////////////////
struct Color{
  Color( void ) : r( 1 ), g( 1 ), b( 1 ), a( 1 ){}
  Color( fpnum rp, fpnum gp, fpnum bp ) : r( rp ), g( gp ), b( bp ), a( 1 ){}
  Color( fpnum rp, fpnum gp, fpnum bp, fpnum ap ) : r( rp ), g( gp ), b( bp ), a( ap ){}
  ~Color( void ){}

  fpnum r;
  fpnum g;
  fpnum b;
  fpnum a;
};
// Fog as per opengl.
struct Fog{
  enum Type { none, linear, exp, exp2 } type;					     
  Color color;
  fpnum start, end, density;

  Fog( Type t, const Color& c, fpnum s, fpnum e, fpnum d ) : type( t ), color( c ),
							     start( s ), end( e ), density( d ){}
  Fog( void ) : type( none ), color( Color( 1, 1, 1, 1 ) ), start( 0 ), end( 1 ), density( 1 ){}
};



#endif //LIGHTING_H
