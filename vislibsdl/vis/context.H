////////////////////////////////////////////////
// Visualization v2 context header.
// (c) Jon DuBois 2004
// 10/22/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef CONTEXT_H
#define CONTEXT_H
#include <iostream>
#include <GL/gl.h>
#include <GL/GLext.h>


/////////////////////////////////////////////
// Views.
/////////////////////////////////////////////
struct View{
  View( word w, word h ) : viewport( Point( 0, 0 ), Point( w, h ) ), rotation( 0, 0, 0 ),
			   position( 0, 0, 1 ), fov( 45 ), clipnear( 1 ), clipfar( 2048 ){}
  View( void ) : rotation( 0, 0, 0 ), position( 0, 0, 1 ), fov( 45 ), clipnear( 1 ), clipfar( 2048 ){}
  
  Region viewport;
  Angle rotation;
  Vector position;
  fpnum fov;
  fpnum clipnear, clipfar;
};



////////////////////////////////////////////
// 
////////////////////////////////////////////



///////////////////////////////////
// The display.
///////////////////////////////////
class Display{
public:

  Display ( word, word, const char* );
  ~Display ( void );

  void draw( Drawable& );
  void texture( const RGBSurface& );
  void bumpmap( const RGBSurface& );
  void emissionmap( const RGBSurface& );
  void specularmap( const RGBSurface& );

  View view( void ) const;
  void view( const View& );

  void fog( const Fog& );
  Fog fog( void ) const;

  void update( void );
  void clear( const Color& );

  fpnum aspect( void );
  dword width( void );
  dword height( void );

  Vector light;
  // Shadowlevel is the darkness of shadows.
  Color shadowcolor;
  bool anisotropic;


private:
  void look( void );
  void loadnormalmap( void );
  void loadgltex(  const RGBSurface& srf );

  Vector *trns, *plght, *lght, *hav;
  bool* visibility;
  size_t tsz, vissize;

  Vector* shadowverts;
  size_t numshadows, maxshadows;

  Fog cfog;

  PFNGLACTIVETEXTUREARBPROC glptrActiveTexture;
  PFNGLMULTITEXCOORD3FARBPROC glptrMultiTexCoord3f;
  PFNGLMULTITEXCOORD2FARBPROC glptrMultiTexCoord2f;

  GLuint nmname, emname, spname;
  bool& exists( void ) const{ static bool e; return e; } 
  static View& vw( void ){ static View v; return v; }
  word bpp;
};


#endif //CONTEXT_H
