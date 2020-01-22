////////////////////////////////////////////////
// Visualization v2 context header.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef CONTEXT_H
#define CONTEXT_H
#include <iostream>
#include <windows.h>
#include <GL/gl.h>
#include "vis/glext.H"



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



///////////////////////////////////
// The display.
///////////////////////////////////
class Display{
public:

  Display ( void );
  ~Display ( void );

  // info
  const char* glrenderer( void ) const;
  const char* glvendor( void ) const;
  const char* glversion( void ) const;
  const char* glextensions( void ) const;
  // cg information
  const char* cgvtexturecompile( void ) const;
  const char* cgftexturecompile( void ) const;

  void draw( Render& );
 
  void texture( const Texture& );
  void bumpmap( const Texture& );
  void emissionmap( const Texture& );
  void specularmap( const Texture& );

  View view( void ) const;
  void view( const View& );

  void fog( const Fog& );
  Fog fog( void ) const;

  void update( void );
  void clear( const Color& );

  static bool anisotropic( void ){ return panisotropic(); }
  static void anisotropic( bool ani ){ panisotropic() = ani; } 

  Vector light;
  // This is the shadow color.
  Color shadowcolor;
  enum Wireframe { none, normal, shadows, both } wireframe;
  Color wireframecolor, shadowwireframecolor;

  friend class Engine;
  friend class Drawable;
  friend class Texture;
private:
  
  // This draws all the registered Renders, storing the shadow producing renders and lights in the
  // second and third argument.
  void drawnormal( Stack< Render* >&, Stack< Vector >& );
  // This does the shadowing.
  void drawshadow( Stack< Render* >&, Stack< Vector >& );

  static bool& panisotropic( void ){ static bool pa = false; return pa; }

  void init( HDC );
  void close( void );
  void look( void );
  static void checkglerror( const char* );

  Stack< Render* > renders;

  // lightvecs is an array for containing all the objectspace light vectors,
  // visibility is an array to keep track of face visibility, tsz and vissize are the
  // sizes of the arrays.
  Vector* lightvecs;
  bool* visibility;
  size_t lightvecsize, vissize;

  Fog cfog;

  static PFNGLACTIVETEXTUREARBPROC glptrActiveTexture;
  static PFNGLMULTITEXCOORD3FARBPROC glptrMultiTexCoord3f;
  static PFNGLMULTITEXCOORD2FARBPROC glptrMultiTexCoord2f;
  static PFNGLCLIENTACTIVETEXTUREPROC glptrClientActiveTexture;

  static PFNGLBINDBUFFERPROC glptrBindBuffer;
  static PFNGLGENBUFFERSPROC glptrGenBuffers;
  static PFNGLDELETEBUFFERSPROC glptrDeleteBuffers;
  static PFNGLBUFFERDATAPROC glptrBufferData;

  static bool canusebuffers;

  HDC devicecontext;

  bool& exists( void ) const{ static bool e; return e; } 
  static View& vw( void ){ static View v; return v; }
  word bpp;
};


#endif //CONTEXT_H
