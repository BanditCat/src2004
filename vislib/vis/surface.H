////////////////////////////////////////////////
// Visualization v2 surface header.
// (c) Jon DuBois 2004
// 10/23/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef SURFACE_H
#define SURFACE_H
#include <iostream>
#include <GL/gl.h>
#include "vis/glext.H"
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
  Pixel( byte pr, byte pg, byte pb ) : r( pr ), g( pg ), b( pb ), a( 255 ){}
  Pixel( byte pr, byte pg, byte pb, byte pa ) : r( pr ), g( pg ), b( pb ), a( pa ){}
  byte r, g, b, a;
};



class Texture;
////////////////////////////////////////////////
// Surfaces
////////////////////////////////////////////////
class Surface{
public:
  Surface( void ){}
  virtual ~Surface( void ){}

  typedef enum Type { mono, greyscale, rgb, rgba };

  size_t width( void ) const{ return pwidth; }
  size_t height( void ) const{ return pheight; }

  virtual void setpixel( const Point&, const Pixel& ) = 0;
  virtual Type type( void ) const = 0;
  virtual bool load( const char* ) = 0;

  friend class Texture;
protected:
  virtual GLenum glformat( void ) const = 0;
  virtual GLenum gliformat( void ) const = 0;
  size_t pwidth, pheight;
  byte* data;
};



////////////////////////////////////////////////
// RGBSurface
////////////////////////////////////////////////
class RGBSurface : public Surface{
public:
  RGBSurface( size_t w, size_t h );
  RGBSurface( const char* fn );
  ~RGBSurface( void );

  size_t width( void ) const{ return pwidth; }
  size_t height( void ) const{ return pheight; }
  
  void setpixel( const Point&, const Pixel& );
  bool load( const char* );
  Surface::Type type( void ) const{ return Surface::rgb; }

  // This turns a heightmap into a bumpmap, height is the relative 
  // depth of the map, a height of one implies a coordinate cube.
  void makebumpmap( fpnum height );

protected:
  GLenum glformat( void ) const{ return GL_RGB; }
  GLenum gliformat( void ) const{ return GL_RGB; }
};



///////////////////////////////////////////////
// Texture
///////////////////////////////////////////////
class Texture{
public:
  Texture( void );
  Texture( const Surface& );
  Texture( const char* );
  ~Texture( void );
  
  size_t width( void ) const{ return pwidth; }
  size_t height( void ) const{ return pheight; }

  bool load( const Surface& );
  bool load( const char* );

  friend class Display;
private:
  size_t pwidth, pheight;
  GLuint glname;
  
  bool loadgltexture( const Surface& );
};



#endif //SURFACE_H
