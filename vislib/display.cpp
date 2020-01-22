//////////////////////////////////////////////////////
// vis v2 context source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <GL/gl.h>
#include "vis/glext.H"
#include <windows.h>
#include "vis/assess.H"
#include "vis/vis.H"


using namespace std; 



PFNGLACTIVETEXTUREARBPROC Display::glptrActiveTexture = NULL;
PFNGLMULTITEXCOORD3FARBPROC Display::glptrMultiTexCoord3f = NULL;
PFNGLMULTITEXCOORD2FARBPROC Display::glptrMultiTexCoord2f = NULL;
PFNGLCLIENTACTIVETEXTUREPROC Display::glptrClientActiveTexture = NULL;

PFNGLBINDBUFFERPROC Display::glptrBindBuffer = NULL;
PFNGLGENBUFFERSPROC Display::glptrGenBuffers = NULL;
PFNGLDELETEBUFFERSPROC Display::glptrDeleteBuffers = NULL;
PFNGLBUFFERDATAPROC Display::glptrBufferData = NULL;

bool Display::canusebuffers = false;



/////////////////////////////////////////////////
// Shaders.
/////////////////////////////////////////////////
/////////////////////////////////////////////
// CGVTexture
//
// This is the texture pass vertex shader.
/////////////////////////////////////////////
class CGVTexture : public CGShader{
public:
  CGVTexture( const char* filename ) : CGShader( filename ){}
  ~CGVTexture( void ){}
  
  void light( const Vector& );
  void view( const Vector& );

private:
  void init( void );
  void pbind( void );
  CGShader::Type type( void ){ return vertex; }

  CGparameter lightp, viewp, onep, modelviewprojp;
} cgvtexture( "shaders/vtexture.cg" );



/////////////////////////////////////////////
// CGFTexture
//
// This is the texture pass fragment shader.
/////////////////////////////////////////////
class CGFTexture : public CGShader{
public:
  CGFTexture( const char* filename ) : CGShader( filename ){}
  ~CGFTexture( void ){}

  void specular( fpnum );

private:
  void init( void );
  void pbind( void );
  CGShader::Type type( void ){ return fragment; }

  CGparameter specularp;
} cgftexture( "shaders/ftexture.cg" );



inline void CGVTexture::light( const Vector& vec ){
  cgGLSetParameter3f( lightp, vec.x, vec.y, vec.z );
}
inline void CGVTexture::view( const Vector& vec ){
  cgGLSetParameter3f( viewp, vec.x, vec.y, vec.z );
}
inline void CGVTexture::init( void ){
  modelviewprojp = cgGetNamedParameter( cgprogram, "modelViewProj" );
  onep = cgGetNamedParameter( cgprogram, "one" );
  lightp = cgGetNamedParameter( cgprogram, "light" );
  viewp = cgGetNamedParameter( cgprogram, "view" );
  if( !modelviewprojp || !onep || !lightp || !viewp || cgcheckerror() )
    cgerror( "Failed to get a parameter for the texture pass vertex shader with this error:\n" );
   
  cgGLSetParameter1f( onep, 1 );
  if( cgcheckerror() )
    cgerror( "Error while setting constants for the texture pass vertex shader:\n" );
}
inline void CGVTexture::pbind( void ){
  cgGLSetStateMatrixParameter( modelviewprojp, CG_GL_MODELVIEW_PROJECTION_MATRIX, CG_GL_MATRIX_IDENTITY );
}



inline void CGFTexture::specular( fpnum spec ){
  cgGLSetParameter1f( specularp, spec );
}
inline void CGFTexture::init( void ){
  specularp = cgGetNamedParameter( cgprogram, "specExponent" );
  if( !specularp || cgcheckerror() )
    cgerror( "Failed to get a parameter for the texture pass fragment shader with this error:\n" );
}
inline void CGFTexture::pbind( void ){}



Display::Display( void ) : light( 0, 0, 0 ),
			   shadowcolor( 0, 0, 0, 0.25 ), 
			   wireframe( none ), 
			   wireframecolor( Color( 1, 1, 1, 0.75 ) ), 
			   shadowwireframecolor( Color( 0, 0, 0, 0.75 ) ),
			   lightvecs( new Vector[ 1 ] ),
			   visibility( new bool[ 1 ] ),
			   lightvecsize( 1 ), vissize( 1 ){
  
#ifdef DBG
  cout << "Constructing display" << endl;
#endif
  if( exists() )
    throw vis_error( "Attempt to double instantiate the display" );
  exists() = true;

}
void Display::init( HDC context ){
#ifdef DBG
  cout << "Display::initing" << endl;
#endif

  devicecontext = context;

  // Get opengl proc addresses
  glptrActiveTexture = (PFNGLACTIVETEXTUREARBPROC)wglGetProcAddress( "glActiveTextureARB" );
  glptrMultiTexCoord3f = (PFNGLMULTITEXCOORD3FARBPROC)wglGetProcAddress( "glMultiTexCoord3fARB" );
  glptrMultiTexCoord2f = (PFNGLMULTITEXCOORD2FARBPROC)wglGetProcAddress( "glMultiTexCoord2fARB" );
  glptrClientActiveTexture = (PFNGLCLIENTACTIVETEXTUREPROC)wglGetProcAddress( "glClientActiveTextureARB" );

  glptrBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress( "glBindBufferARB" );
  glptrGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress( "glGenBuffersARB" );
  glptrDeleteBuffers = (PFNGLDELETEBUFFERSPROC)wglGetProcAddress( "glDeleteBuffersARB" );
  glptrBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress( "glBufferDataARB" );

  if( !glptrMultiTexCoord3f || !glptrMultiTexCoord2f || !glptrMultiTexCoord3f )
    throw vis_environment_error( "Failed to get proccess address for required OpenGL extensions" );
  if( glptrBindBuffer && glptrGenBuffers && glptrDeleteBuffers && glptrBufferData )
    canusebuffers = true;


  // Check for at least four texture units( required for bump mapping )
  {
    GLint texunits;
    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &texunits);
      if( texunits < 4 )
    throw vis_environment_error( "Not enough texture units (less than four)" );
  }

  // do openGL init
#ifdef DBG
  cout << "Initing GL with a " << vw().viewport.width() << "x" << vw().viewport.height() << " window" << endl;
#endif
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glViewport( 0, 0, vw().viewport.width(), vw().viewport.height() ); 

  glShadeModel( GL_SMOOTH );
  glClearColor( 0, 0, 0, 0 );	
  
  glClearDepth( 1.0f );		
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

  glFrontFace( GL_CCW );
  glCullFace( GL_BACK );
  glEnable( GL_CULL_FACE );

  glEnable( GL_BLEND );

  glEnable( GL_DITHER );

  glEnable( GL_MULTISAMPLE );

  glColor4f( 1, 1, 1, 1 );

  //  Load default textures
  static Texture bumptex, diffusetex, emissiontex, speculartex;
  {
    RGBSurface surf( 1, 1 );
    surf.setpixel( Point( 0, 0 ), Pixel( 127, 127, 255 ) );
    bumptex.load( surf );
    surf.setpixel( Point( 0, 0 ), Pixel( 255, 255, 255 ) );
    diffusetex.load( surf );
    surf.setpixel( Point( 0, 0 ), Pixel( 0, 0, 0 ) );
    emissiontex.load( surf );
    surf.setpixel( Point( 0, 0 ), Pixel( 96, 96, 96 ) );
    speculartex.load( surf );
    bumpmap( bumptex );
    texture( diffusetex );
    emissionmap( emissiontex );
    specularmap( speculartex );
  }
  
  checkglerror( "Failed to initialize OpenGL with this error : " );

  CGShader::cginit();
  // We bind these here to force them to get compiled now.
  cgvtexture.bind();
  cgftexture.bind();
  CGShader::disable();
}
Display::~Display( void ){
#ifdef DBG
  cout << "Deconstructing display" << endl;
#endif
  delete[] lightvecs;
  delete[] visibility;

  exists() = false;
}
void Display::close( void ){
#ifdef DBG
  cout << "Closing display with Display::close()" << endl;
#endif
  CGShader::cgclose();
}



const char* Display::glrenderer( void ) const{ return (const char*)glGetString( GL_RENDERER ); }
const char* Display::glvendor( void ) const{ return (const char*)glGetString( GL_VENDOR ); }
const char* Display::glversion( void ) const{ return (const char*)glGetString( GL_VERSION ); }
const char* Display::glextensions( void ) const{ return (const char*)glGetString( GL_EXTENSIONS ); }
const char* Display::cgvtexturecompile( void ) const{ return cgvtexture.listing(); }
const char* Display::cgftexturecompile( void ) const{ return cgftexture.listing(); }



void Display::draw( Render& ren ){
#ifdef DBG
  cout << "Display::drawing" << endl;
#endif
  renders.push( &ren );
}
void Display::texture( const Texture& tex ){
  glptrActiveTexture( GL_TEXTURE2 );
  glBindTexture( GL_TEXTURE_2D, tex.glname );
}
void Display::bumpmap( const Texture& tex ){
  glptrActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, tex.glname );
}
void Display::emissionmap( const Texture& tex ){
  glptrActiveTexture( GL_TEXTURE0 );
  glBindTexture( GL_TEXTURE_2D, tex.glname );
}
void Display::specularmap( const Texture& tex ){
  glptrActiveTexture( GL_TEXTURE3 );
  glBindTexture( GL_TEXTURE_2D, tex.glname );
}



View Display::view( void ) const{
  return vw();
}
void Display::view( const View& v ){
  vw() = v;
  look();
}

void Display::fog( const Fog& f ){
  if( f.type == Fog::none )
    glDisable( GL_FOG );
  else
    glEnable( GL_FOG );
  if( f.type == Fog::linear )
    glFogi( GL_FOG_MODE, GL_LINEAR );
  if( f.type == Fog::exp )
    glFogi( GL_FOG_MODE, GL_EXP );
  if( f.type == Fog::exp2 )
    glFogi( GL_FOG_MODE, GL_EXP2 );
  glFogf( GL_FOG_DENSITY, f.density );
  glFogf( GL_FOG_START, f.start );
  glFogf( GL_FOG_END, f.end );
  fpnum tfa[ 4 ] = { f.color.r, f.color.g, f.color.b, f.color.a };
  glFogfv( GL_FOG_COLOR, tfa ); 

  cfog = f;
}
Fog Display::fog( void ) const{
  return cfog;
}

void Display::update( void ){
#ifdef DBG
  cout << "Display::updating" << endl;
#endif

  static Stack< Render* > shadowrenders;
  static Stack< Vector > shadowlights;

  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  
  glRotatef( -vw().rotation.z, 0, 0, 1 );
  glRotatef( -vw().rotation.x, 1, 0, 0 );
  glRotatef( -vw().rotation.y, 0, 1, 0 );
  
  glTranslatef( -vw().position.x, -vw().position.y, -vw().position.z );
  
  drawnormal( shadowrenders, shadowlights );
  drawshadow( shadowrenders, shadowlights );
 
  checkglerror( "OpenGL reported this error while updating the screen : " );
  
  SwapBuffers( devicecontext );
}
void Display::clear( const Color& clr ){
#ifdef DBG
  cout << "Display::clearing" << endl;
#endif
  glClearColor( clr.r, clr.g, clr.b, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  glClearColor( 0, 0, 0, 0 );
}



void Display::drawnormal( Stack< Render* >& shadowrenders, Stack< Vector >& shadowlights ){
  while( renders.size() ){
    Render& ren = *renders.peek();
    Drawable& d = *ren.object;
    
    if( ren.texture != NULL )
      texture( *ren.texture );
    if( ren.bumpmap != NULL )
      bumpmap( *ren.bumpmap );
    if( ren.specularmap != NULL )
      specularmap( *ren.specularmap );
    
    Matrix *tmat( &ren.transform );
    tmat->invert();
    Vector tlight( light );
    tlight.transform( *tmat );
    Vector tview( vw().position );
    tview.transform( *tmat );
    tmat->invert();
    
    glPushMatrix();
    glMultMatrixf( (*tmat)[ 0 ] );
    
    cgvtexture.bind();
    cgftexture.bind();
    // Set the light and view vectors, and the specular exponent
    cgvtexture.light( Vector( tlight.x, tlight.y, tlight.z ) );
    cgvtexture.view( Vector( tview.x, tview.y, tview.z ) );
    cgftexture.specular( ren.specular );
    
    if( d.compiled() ){
      glptrBindBuffer( GL_ARRAY_BUFFER_ARB, d.vertexbuffer );
      glVertexPointer( 3, GL_FLOAT, 0, NULL );
      glEnableClientState( GL_VERTEX_ARRAY );
      
      glptrBindBuffer( GL_ARRAY_BUFFER_ARB, d.xybuffer );
      glptrClientActiveTexture( GL_TEXTURE0 );
      glTexCoordPointer( 2, GL_FLOAT, 0, NULL );
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
      glptrBindBuffer( GL_ARRAY_BUFFER_ARB, d.sbuffer );
      glptrClientActiveTexture( GL_TEXTURE1 );
      glTexCoordPointer( 3, GL_FLOAT, 0, NULL );
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
      glptrBindBuffer( GL_ARRAY_BUFFER_ARB, d.tbuffer );
      glptrClientActiveTexture( GL_TEXTURE2 );
      glTexCoordPointer( 3, GL_FLOAT, 0, NULL );
      glEnableClientState( GL_TEXTURE_COORD_ARRAY );
      
      glptrBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, d.indexbuffer );
      glDrawElements( GL_TRIANGLES, d.facecount() * 3, GL_UNSIGNED_INT, NULL );
      
      glDisableClientState( GL_VERTEX_ARRAY );
      glDisableClientState( GL_TEXTURE_COORD_ARRAY );
      glptrClientActiveTexture( GL_TEXTURE1 );
      glDisableClientState( GL_TEXTURE_COORD_ARRAY );
      glptrClientActiveTexture( GL_TEXTURE0 );
      glDisableClientState( GL_TEXTURE_COORD_ARRAY );
    } else {
      
      glBegin( GL_TRIANGLES );
      // Draw each triangle.
      for( size_t i = 0; i < d.facecount(); i++ ){
	for( size_t j = 0; j < 3; j++ ){
	  size_t texindex = d.textures[ i ][ j ];
	  size_t index = d.hull->faces[ i ][ j ];
	  fpnum s = d.texcoords[ texindex ].x;
	  fpnum t = d.texcoords[ texindex ].y;
	  glTexCoord2f( s, t );
	  Vector texs = d.texcoords[ texindex ].s;
	  Vector text = d.texcoords[ texindex ].t;
	  // Communicate in texture space.
	  glptrMultiTexCoord3f( GL_TEXTURE1, texs.x, texs.y, texs.z );
	  glptrMultiTexCoord3f( GL_TEXTURE2, text.x, text.y, text.z );
	  glVertex3f( d.hull->vertices[ index ].x, d.hull->vertices[ index ].y, d.hull->vertices[ index ].z );
	}
      }
      glEnd();
    }
    
    CGShader::disable();
    
    if( ( wireframe == normal ) || ( wireframe == both ) ){
      glColor4f( wireframecolor.r, wireframecolor.g, wireframecolor.b, wireframecolor.a );
      glDisable( GL_DEPTH_TEST );
      for( size_t i = 0; i < d.facecount(); i++ ){
	glBegin( GL_LINE_STRIP );
	for( size_t j = 0; j < 4; j++ ){
	  size_t index = d.hull->faces[ i ][ j % 3 ];
	  glVertex3f( d.hull->vertices[ index ].x, d.hull->vertices[ index ].y, d.hull->vertices[ index ].z );
	}
	glEnd();
      }
      glEnable( GL_DEPTH_TEST );
    }
 
    glPopMatrix();
    
    
    // do shadowing if shadowdist > 0;
    if( ( ren.shadowdepth > 0 ) && ( shadowcolor.a > 0 ) ){
      shadowlights.push( tlight );
      shadowrenders.push( &ren );
    }

    renders.pop();
  }
}
void Display::drawshadow( Stack< Render* >& shadowrenders, Stack< Vector >& shadowlights ){
  if( ( shadowcolor.a > 0 ) && shadowrenders.size() ){

    // Set up for stencil passes.
    glDepthFunc( GL_GEQUAL );
    glDepthMask( GL_FALSE );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );

    glEnable( GL_STENCIL_TEST );
  
    while( shadowrenders.size() ){
      Render& render = *shadowrenders.peek();
      Drawable& drawable = *render.object;

      if( drawable.vertexcount() > lightvecsize ){
	lightvecsize = drawable.vertexcount();
	delete[] lightvecs;
	lightvecs = new Vector[ lightvecsize ];
      }
      
      if( drawable.facecount() > vissize ){
	vissize = drawable.facecount();
	delete[] visibility;
	visibility = new bool[ vissize ];
      }
    
      for( size_t i = 0; i < drawable.vertexcount(); i++ ){
	lightvecs[ i ] = shadowlights.peek();
	lightvecs[ i ] -= drawable.hull->vertices[ i ];
	lightvecs[ i ].normal();
      }

      // This stores all the triangles to be drawn.
      static Stack< Vector > sstack;

      // Find faces that the light shines on and set visibility.
      for( size_t i = 0; i < drawable.facecount(); i++ )
	if( lightvecs[ drawable.hull->faces[ i ][ 0 ] ].dot( drawable.hull->normals[ i ] ) > 0 ){
	  sstack.push( drawable.hull->vertices[ drawable.hull->faces[ i ][ 2 ] ] );
	  sstack.push( drawable.hull->vertices[ drawable.hull->faces[ i ][ 1 ] ] );
	  sstack.push( drawable.hull->vertices[ drawable.hull->faces[ i ][ 0 ] ] );
	  sstack.push( lightvecs[ drawable.hull->faces[ i ][ 0 ] ] );
	  sstack.peek() *= -render.shadowdepth;
	  sstack.push( lightvecs[ drawable.hull->faces[ i ][ 1 ] ] );
	  sstack.peek() *= -render.shadowdepth;
	  sstack.push( lightvecs[ drawable.hull->faces[ i ][ 2 ] ] );
	  sstack.peek() *= -render.shadowdepth;
	  visibility[ i ] = true;
	} else
	  visibility[ i ] = false;

      // Now push the vertices into shadowverts
      for( size_t i = 0; i < drawable.facecount(); i++ ){
	// Look at each neighbor if this face is visible
	if( visibility[ i ] ){
	  for( size_t j = 0; j < 3; j++ ){
	    //check to see if this has a neighbor that isn't visible.
	    if( !visibility[ drawable.neighbors[ i ][ j ] ] || 
		( drawable.neighbors[ i ][ j ] == i ) ){
	      // Push the four corners onto shadowverts
	      size_t index = drawable.hull->faces[ i ][ j ];
	      Vector corner1 = drawable.hull->vertices[ index ];
	      Vector corner2 = lightvecs[ index ];
	      corner2 *= -render.shadowdepth;
	      index = drawable.hull->faces[ i ][ ( j + 1 ) % 3 ];
	      Vector corner3 = drawable.hull->vertices[ index ];
	      Vector corner4 = lightvecs[ index ];
	      corner4 *= -render.shadowdepth;
	      sstack.push( corner3 );
	      sstack.push( corner2 );
	      sstack.push( corner1 );
	      sstack.push( corner2 );
	      sstack.push( corner3 );
	      sstack.push( corner4 );
	    }
	  }
	}
      }

      glPushMatrix();
      glMultMatrixf( render.transform[ 0 ] );

      // Draw wire frames
      if( ( wireframe == shadows ) || ( wireframe == both ) ){
	glDisable( GL_DEPTH_TEST );
	glDisable( GL_STENCIL_TEST );
	glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
	glColor4f( shadowwireframecolor.r, shadowwireframecolor.g, shadowwireframecolor.b, shadowwireframecolor.a );
	for( size_t i = 0; i < sstack.size(); i += 3 ){
	  glBegin( GL_LINE_STRIP );
	  glVertex3f( sstack[ i + 2 ].x, sstack[ i + 2 ].y, sstack[ i + 2 ].z );
	  glVertex3f( sstack[ i + 1 ].x, sstack[ i + 1 ].y, sstack[ i + 1 ].z );
	  glVertex3f( sstack[ i + 0 ].x, sstack[ i + 0 ].y, sstack[ i + 0 ].z );
	  glVertex3f( sstack[ i + 2 ].x, sstack[ i + 2 ].y, sstack[ i + 2 ].z );
	  glEnd();
	}      
	glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
	glEnable( GL_STENCIL_TEST );
	glEnable( GL_DEPTH_TEST );
      }

      // Turn "on" the shadows.
      glFrontFace( GL_CW );
      glStencilFunc( GL_ALWAYS, 0, 0xffffffff );
      glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
      for( size_t i = 0; i < sstack.size(); i += 3 ){
	glBegin( GL_TRIANGLES );
	glVertex3f( sstack[ i + 2 ].x, sstack[ i + 2 ].y, sstack[ i + 2 ].z );
	glVertex3f( sstack[ i + 1 ].x, sstack[ i + 1 ].y, sstack[ i + 1 ].z );
	glVertex3f( sstack[ i + 0 ].x, sstack[ i + 0 ].y, sstack[ i + 0 ].z );
	glEnd();
      }

      // Turn "off" the backside of the shadows
      glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
      glFrontFace( GL_CCW );
      while( sstack.size() ){
	glBegin( GL_TRIANGLES );
	glVertex3f( sstack.peek().x, sstack.peek().y, sstack.peek().z );
	sstack.pop();
	glVertex3f( sstack.peek().x, sstack.peek().y, sstack.peek().z );
	sstack.pop();
	glVertex3f( sstack.peek().x, sstack.peek().y, sstack.peek().z );
	sstack.pop();
	glEnd();
      }

      glPopMatrix();
      shadowrenders.pop();
      shadowlights.pop();
    }
    glFrontFace( GL_CCW );

    // Set up for shadow rectangle.
    glDepthFunc( GL_LEQUAL );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
    glStencilFunc( GL_NOTEQUAL, 0, 0xffffffff );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    // draw in the shadows.
    glColor4f(  shadowcolor.r,  shadowcolor.g,  shadowcolor.b, shadowcolor.a );
    glBegin( GL_TRIANGLE_STRIP );
    glVertex2f( -1, -1 );
    glVertex2f( 1, -1 );
    glVertex2f( -1, 1 );
    glVertex2f( 1, 1 );
    glEnd();

    //Set up for normal writing.
    glDisable( GL_STENCIL_TEST );
    glDepthMask( GL_TRUE );

  }
}



void Display::look( void ){

#ifdef DBG
  cout << "Display::looking: width=" << vw().viewport.width() << 
    " height=" << vw().viewport.height() << endl;
#endif
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glViewport( 0, 0, vw().viewport.width(), vw().viewport.height() );
  fpnum up = tanf( vw().fov * torad / 2 ) * vw().clipnear;
  fpnum right = up * ( (fpnum)vw().viewport.width() / (fpnum)vw().viewport.height() );
  glFrustum( -right, right, -up, up, vw().clipnear, vw().clipfar );
}
void Display::checkglerror( const char* message ){
  static char errmsg[ 1024 ];
  const char* bmsg = ( message == NULL ) ? "An openGL error has occured : " : message;
  strncpy( errmsg, bmsg, 1023 );
  errmsg[ 1023 ] = '\000';
  char* emsg;

  switch( glGetError() ){
  case GL_INVALID_ENUM:
    emsg = "GL_INVALID_ENUM";
    break;
  case GL_INVALID_VALUE:
    emsg = "GL_INVALID_VALUE";
    break;
  case GL_INVALID_OPERATION:
    emsg = "GL_INVALID_OPERATION";
    break;
  case GL_STACK_OVERFLOW:
    emsg = "GL_STACK_OVERFLOW";
    break;
  case GL_STACK_UNDERFLOW:
    emsg = "GL_STACK_UNDERFLOW";
    break;
  case GL_OUT_OF_MEMORY:
    emsg = "GL_OUT_OF_MEMORY";
    break;
  case GL_TABLE_TOO_LARGE:
    emsg = "GL_TABLE_TOO_LARGE";
    break;
  default:
    emsg = NULL;
    break;
  }
  if( emsg != NULL ){
    strncat( errmsg, emsg, 1023 - strlen( errmsg ) );
    throw vis_environment_error( errmsg );
  }
}
