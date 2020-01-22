//////////////////////////////////////////////////////
// vis v2 surface source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <GL/gl.h>
#include <windows.h>
#include "vis/assess.H"
#include "vis/vis.H"

using namespace std; 



RGBSurface::RGBSurface( size_t w, size_t h ){
  pwidth = w;
  pheight = h;
  data = new byte[ w * h * 3 ];
}
RGBSurface::RGBSurface( const char* filename ){
  data = new byte[ 1 ];
  if( !load( filename ) ){
    static char errmsg[ 512 ];
    char* begining = "Failed to load surface from file ";
    if( ( strlen( filename ) + strlen( begining ) ) > 511 )
      throw vis_bad_load( "Failed to load surface from file" );
    else{
      strcpy( errmsg, begining );
      strcat( errmsg, filename );
      throw vis_bad_load( errmsg );
    }
  } 
}
RGBSurface::~RGBSurface( void ){
  delete[] data;
}



void RGBSurface::setpixel( const Point& c, const Pixel& p ){
  byte* ofs = data + ( c.x + ( ( pheight - c.y ) - 1 ) * pwidth ) * 3;
  *ofs++ = p.r;
  *ofs++ = p.g;
  *ofs++ = p.b;
}
bool RGBSurface::load( const char* filename ){
  delete[] data;

  BITMAP bitmap;
  HBITMAP bitmaphandle;

  bitmaphandle = (HBITMAP)LoadImage( GetModuleHandle(NULL), filename, IMAGE_BITMAP, 
				     0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE );

  if( !bitmaphandle )
    return false;
      
  GetObject( bitmaphandle, sizeof( bitmap ), &bitmap );

  pwidth = bitmap.bmWidth;
  pheight = bitmap.bmHeight;
  data = new byte[ pwidth * pheight * 3 ];

  byte* dest = data;
  byte* source = (byte*)bitmap.bmBits;
  byte* end = dest + pwidth * pheight * 3;
  while( dest < end ){
    dest[ 0 ] = source[ 2 ];
    dest[ 1 ] = source[ 1 ];
    dest[ 2 ] = source[ 0 ];
    dest += 3;
    source += 3;
  }
  
  DeleteObject( bitmaphandle );

  return true;
}
void RGBSurface::makebumpmap( fpnum height ){
  fpnum dim = ( pwidth > pheight ) ? pheight : pwidth;
  Vector u( 0, 0, 0 );
  Vector v( 0, 0, 0 );
  byte *z1, *z2;
  byte* newmap = new byte[ pwidth * pheight * 3 ];
  for( size_t y = 0; y < pheight; y++ ){
    for( size_t x = 0; x < pwidth; x++ ){
      if( x == 0 ){
	z1 =  data + ( x + y * pwidth ) * 3;
	z2 =  data + ( ( x + 1 ) + y * pwidth ) * 3;
	u.x = 1;
      } else if( ( x + 1 ) < pwidth ){
	z1 =  data + ( ( x - 1 ) + y * pwidth ) * 3;
	z2 =  data + ( ( x + 1 ) + y * pwidth ) * 3;
	u.x = 2;
      } else {
	z1 =  data + ( ( x - 1 ) + y * pwidth ) * 3;
	z2 =  data + ( x  + y * pwidth ) * 3;
	u.x = 1;
      }
      fpnum zf1 = ( (fpnum)*z1++ ) + ( (fpnum)*z1++ ) + ( (fpnum)*z1 );
      fpnum zf2 = ( (fpnum)*z2++ ) + ( (fpnum)*z2++ ) + ( (fpnum)*z2 );
      u.z = ( zf1 - zf2 ) * height * dim / 765.0f;
      if( y == 0 ){
	z1 =  data + ( x + y * pwidth ) * 3;
	z2 =  data + ( x + ( y + 1 ) * pwidth ) * 3;
	v.y = 1;
      } else if( ( y + 1 ) < pheight ){
	z1 =  data + ( x + ( y - 1 ) * pwidth ) * 3;
	z2 =  data + ( x + ( y + 1 ) * pwidth ) * 3;
	v.y = 2;
      } else {
	z1 =  data + ( x + ( y - 1 ) * pwidth ) * 3;
	z2 =  data + ( x + y * pwidth ) * 3;
	v.y = 1;
      }
      zf1 = ( (fpnum)*z1++ ) + ( (fpnum)*z1++ ) + ( (fpnum)*z1 );
      zf2 = ( (fpnum)*z2++ ) + ( (fpnum)*z2++ ) + ( (fpnum)*z2 );
      v.z = ( zf1 - zf2 ) * height * dim / 765.0f;
      
      u.cross( v );
      u.normal();

      byte* dest = newmap + ( x + y * pwidth ) * 3;
      *dest++ = (byte)( ( -u.x + 1 ) * 127.5f );
      *dest++ = (byte)( ( u.y + 1 ) * 127.5f );
      *dest++ = (byte)( ( u.z + 1 ) * 127.5f ); 
    }
  }
  delete[] data;
  data = newmap;
}



Texture::Texture( void ) : pwidth( 0 ), pheight( 0 ){
  glGenTextures( 1, &glname );
}
Texture::Texture( const Surface& surf ){
  pwidth = surf.width();
  pheight = surf.height();

  glGenTextures( 1, &glname );
  if( !loadgltexture( surf ) )
    throw vis_bad_load(  "Failed to create a Texture from a Surface" );
}
Texture::Texture( const char* filename ){
  glGenTextures( 1, &glname );

  RGBSurface surf( 1, 1 );
  if( !surf.load( filename ) ){
    static char errmsg[ 512 ];
    char* begining = "Failed to load texture from file ";
    if( ( strlen( filename ) + strlen( begining ) ) > 511 )
      throw vis_bad_load( "Failed to load texture from file" );
    else{
      strcpy( errmsg, begining );
      strcat( errmsg, filename );
      throw vis_bad_load( errmsg );
    }
  } 
  
  if( !loadgltexture( surf ) )
    throw vis_environment_error( "Failed to create a Texture from a Surface" );

  pwidth = surf.width();
  pheight = surf.height();
}
Texture::~Texture( void ){
  glDeleteTextures( 1, &glname );
}


bool Texture::load( const Surface& surf ){
  return loadgltexture( surf );
}
bool Texture::load( const char* filename ){
  RGBSurface surf( 1, 1 );
  if( !surf.load( filename ) )
    return false;
  return loadgltexture( surf );
}


bool Texture::loadgltexture( const Surface& surf ){
  GLint oldname;
  glGetIntegerv( GL_TEXTURE_BINDING_2D, &oldname );

  glBindTexture( GL_TEXTURE_2D, glname );

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); 
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP_SGIS, GL_TRUE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

  // Set up anisotropic filtering
  if( Display::anisotropic() ){
    GLfloat f;
    glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f );
  } else
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1 );

  glTexImage2D( GL_TEXTURE_2D, 0, surf.gliformat(), surf.width(), surf.height(), 
		0, surf.glformat(), GL_UNSIGNED_BYTE, surf.data );	

  glBindTexture( GL_TEXTURE_2D, oldname );

  Display::checkglerror( "OpenGL reported this error while loading a texture : " );
  return true;
}
