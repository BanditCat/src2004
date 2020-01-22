////////////////////////////////////////////////
// Visualization drawable header.
// (c) Jon DuBois 2004
// 02/25/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef SURFACE_H
#define SURFACE_H
#include <iostream>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include "vis/vis.H"



///////////////////////////////////////
// Points etc.
///////////////////////////////////////
struct Point{
  Point( void ) : x( 0 ), y( 0 ){}
  Point( word xp, word yp ) : x( xp ), y( yp ){}

  word x;
  word y;
};
struct Region{
  Region( void ) : lw( Point( 0, 0 ) ), hgh( Point( 0, 0 ) ){}
  Region( Point p1, Point p2 ){ set( p1, p2 ); }

  Point low( void ) const{ return lw; }
  void low( const Point& p ){ set( p, hgh ); }
  Point high( void ) const{ return hgh; }
  void high( const Point& p ){ set( p, lw ); }
  word width( void ) const{ return hgh.x - lw.x; }
  word height( void ) const{ return hgh.y - lw.y; }
  
private:
  Point lw, hgh;
  void set( const Point& p1, const Point& p2 );
};
inline void Region::set( const Point& p1, const Point& p2 ){
  if( p1.x < p2.x ){ lw.x = p1.x; hgh.x = p2.x; } else { lw.x = p2.x; hgh.x = p1.x; }      
  if( p1.y < p2.y ){ lw.y = p1.y; hgh.y = p2.y; } else { lw.y = p2.y; hgh.y = p1.y; }      
}
struct Pixel{
  Pixel( byte pr, byte pg, byte pb ) : r( pr ), g( pg ), b( pb ){}
  byte r, g, b;
};



////////////////////////////////////////////////
// RGBSurface
////////////////////////////////////////////////
class RGBSurface{
public:
  RGBSurface( size_t w, size_t h );
  RGBSurface( const char* fn );
  ~RGBSurface( void );

  size_t width( void ) const{ return wdth; }
  size_t height( void ) const{ return hght; }
  
  void setpixel( const Point&, const Pixel& );
  // This turns a heightmap into a bumpmap, height is the relative 
  // depth of the map, a height of one implies a coordinate cube.
  void makebumpmap( fpnum height );
  friend class Display;
private:
  SDL_Surface* fliprgbsurf( SDL_Surface* );
  SDL_Surface* torgbsurf( SDL_Surface* );

  bool dirty;
  GLuint glnm;
  byte* data;
  size_t wdth, hght;
};



#endif //SURFACE_H
