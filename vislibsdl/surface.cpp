//////////////////////////////////////////////////////
// vis v2 surface source file.
// (c) Jon DuBois 2004
// 10/22/2004
// This file is licensed via the GPL.
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <SDL/SDL.h>
#include <GL/gl.h>
#include "vis/assess.H"
#include "vis/vis.H"

using namespace std; 


//////////////////////////////////////
// Points etc
//////////////////////////////////////
// struct Point{
//   Point( void ) : x( 0 ), y( 0 ){}
//   Point( word xp, word yp ) : x( xp ), y( yp ){}

//   word x;
//   word y;
// };
// struct Region{
//   Region( void ) : lw( Point( 0, 0 ) ), hgh( Point( 0, 0 ) ){}
//   Region( Point p1, Point p2 ){ set( p1, p2 ); }

//   Point low( void ) const{ return lw; }
//   void low( const Point& p ){ set( p, hgh ); }
//   Point high( void ) const{ return hgh; }
//   void high( const Point& p ){ set( p, lw ); }
//   word width( void ) const{ return hgh.x - lw.x; }
//   word height( void ) const{ return hgh.y - lw.y; }
  
// private:
//   Point lw, hgh;
//   void set( const Point& p1, const Point& p2 );
// };
// inline void Region::set( const Point& p1, const Point& p2 ){
//   if( p1.x < p2.x ){ lw.x = p1.x; hgh.x = p2.x; } else { lw.x = p2.x; hgh.x = p1.x; }      
//   if( p1.y < p2.y ){ lw.y = p1.y; hgh.y = p2.y; } else { lw.y = p2.y; hgh.y = p1.y; }      
// }
// struct Pixel{
//   Pixel( byte pr, byte pg, byte pb ) : r( pr ), g( pg ), b( pb ){}
//   byte r, g, b;
// };


////////////////////////////////////////////////
// RGBSurface
////////////////////////////////////////////////
// class RGBSurface{
// public:
//   RGBSurface( size_t w, size_t h );
//   RGBSurface( const String& fn );
//   ~RGBSurface( void );

//   size_t width( void ) const{ return wdth; }
//   size_t height( void ) const{ return hght; }

//   void setpixel( const Point&, const Pixel& );
//   // This turns a heightmap into a bumpmap, height is the relative 
//   // depth of the map, a height of one implies a coordinate cube.
//   void makebumpmap( fpnum height );
//   friend class Display;
// private:
//   SDL_Surface* fliprgbsurf( SDL_Surface* );
//   SDL_Surface* torgbsurf( SDL_Surface* );

//   GLuint glnm;
//   bool dirty;
//   byte* data;
//   size_t wdth, hght;
// }
RGBSurface::RGBSurface( size_t w, size_t h ) : dirty( true ), data( new byte[ w * h * 3 ] ),
					       wdth( w ), hght( h ){
  glGenTextures( 1, &glnm );
}
RGBSurface::~RGBSurface( void ){
  glDeleteTextures( 1, &glnm );
  delete[] data;
}


void RGBSurface::setpixel( const Point& c, const Pixel& p ){
  dirty = true;
  byte* ofs = data + ( c.x +  c.y * wdth ) * 3;
  *ofs++ = p.r;
  *ofs++ = p.g;
  *ofs++ = p.b;
}
void RGBSurface::makebumpmap( fpnum height ){
  dirty = true;
  fpnum dim = ( wdth > hght ) ? hght : wdth;
  Vector u( 0, 0, 0 );
  Vector v( 0, 0, 0 );
  byte *z1, *z2;
  byte* newmap = new byte[ wdth * hght * 3 ];
  byte* dest = newmap;
  for( size_t y = 0; y < hght; y++ ){
    for( size_t x = 0; x < wdth; x++ ){
      if( x == 0 ){
	z1 =  data + ( x + ( ( hght - y ) - 1 ) * wdth ) * 3;
	z2 =  data + ( ( x + 1 ) + ( ( hght - y ) - 1 ) * wdth ) * 3;
	u.x = 1;
      } else if( ( x + 1 ) < wdth ){
	z1 =  data + ( ( x - 1 ) + ( ( hght - y ) - 1 ) * wdth ) * 3;
	z2 =  data + ( ( x + 1 ) + ( ( hght - y ) - 1 ) * wdth ) * 3;
	u.x = 2;
      } else {
	z1 =  data + ( ( x - 1 ) + ( ( hght - y ) - 1 ) * wdth ) * 3;
	z2 =  data + ( x  + ( ( hght - y ) - 1 ) * wdth ) * 3;
	u.x = 1;
      }
      fpnum zf1 = ( (fpnum)*z1++ ) + ( (fpnum)*z1++ ) + ( (fpnum)*z1 );
      fpnum zf2 = ( (fpnum)*z2++ ) + ( (fpnum)*z2++ ) + ( (fpnum)*z2 );
      u.z = ( zf1 - zf2 ) * height * dim / 765.0f;
      if( y == 0 ){
	z1 =  data + ( x + ( ( hght - y ) - 1 ) * wdth ) * 3;
	z2 =  data + ( x + ( ( hght - ( y + 1 ) ) - 1 ) * wdth ) * 3;
	v.y = 1;
      } else if( ( y + 1 ) < hght ){
	z1 =  data + ( x + ( ( hght - ( y - 1 ) ) - 1 ) * wdth ) * 3;
	z2 =  data + ( x + ( ( hght - ( y + 1 ) ) - 1 ) * wdth ) * 3;
	v.y = 2;
      } else {
	z1 =  data + ( x + ( ( hght - ( y - 1 ) ) - 1 ) * wdth ) * 3;
	z2 =  data + ( x + ( ( hght - y ) - 1 ) * wdth ) * 3;
	v.y = 1;
      }
      zf1 = ( (fpnum)*z1++ ) + ( (fpnum)*z1++ ) + ( (fpnum)*z1 );
      zf2 = ( (fpnum)*z2++ ) + ( (fpnum)*z2++ ) + ( (fpnum)*z2 );
      v.z = ( zf1 - zf2 ) * height * dim / 765.0f;
      
      u.cross( v );
      u.normal();

      dest = newmap + ( x + ( ( hght - y ) - 1 ) * wdth ) * 3;
      *dest++ = (byte)( ( -u.x + 1 ) * 127.5f ); 
      *dest++ = (byte)( ( u.y + 1 ) * 127.5f ); 
      *dest++ = (byte)( ( u.z + 1 ) * 127.5f ); 
    }
  }
  delete[] data;
  data = newmap;
}


SDL_Surface* RGBSurface::torgbsurf( SDL_Surface* s ){
  SDL_PixelFormat pf;
  pf.palette = NULL;
  pf.BitsPerPixel = 24;
  pf.BytesPerPixel = 3;
  pf.Rmask = 255;
  pf.Gmask = (dword)255 << 8;
  pf.Bmask = (dword)255 << 16;
  pf.Amask = 255;
  pf.Rshift = pf.Gshift = pf.Bshift = pf.Ashift = 0;
  pf.Rloss = pf.Gloss = pf.Bloss = pf.Aloss = 0;
  pf.colorkey = 0;
  pf.alpha = 255;

  SDL_Surface* bsrf = SDL_ConvertSurface( s, &pf, 0 );
  if( !bsrf ){
    char em[ 200 ];
    char* beg = "Failed to load create texture with this error: ";
    char* sdle = SDL_GetError();
    if( ( strlen( beg ) + strlen( sdle ) ) > 199 )
      strcpy( em, "Failed to create texture" );
    else{
      strcpy( em, beg );
      strcat( em, sdle );
    }
    throw vis_environment_error( em );
  }

  SDL_FreeSurface( s );
  return bsrf;
}
