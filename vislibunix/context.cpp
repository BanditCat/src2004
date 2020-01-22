//////////////////////////////////////////////////////
// vis context source file.
// (c) Jon DuBois 2004
// 05/04/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <GL/gl.h>
#include <GL/glext.h>
#include <SDL/SDL.h>
#include "vis/assess.H"
#include "vis/vis.H"


using namespace std; 



////////////////////////////////////////////////////////
// The display
////////////////////////////////////////////////////////
// class Display{
// public:
//   Display ( word, word, const char* );
//   ~Display ( void );

//   void draw( Drawable& );
//   void texture( const RGBSurface& );
//   void bumpmap( const RGBSurface& );
//   void emissionmap( const RGBSurface& );
//   void specularmap( const RGBSurface& );

//   View view( void ) const;
//   void view( const View& );

//   void fog( const Fog& );
//   Fog fog( void ) const;

//   void update( void );
//   void clear( const Color& );

//   fpnum aspect( void );
//   dword width( void );
//   dword height( void );

//   Vector light;
//   // Shadowlevel is the darkness of shadows.
//   Color shadowcolor;
//   bool anisotropic;
// private:
//   void look( void );
//   void loadnormalmap( void );
//   void loadgltex( const RGBSurface& srf );

//   Vector *trns, *plght, *lght, *hav;
//   bool* visibility;
//   fpnum* fogdepths;
//   size_t tsz, vissize;

//   Vector* shadowverts;
//   size_t numshadows, maxshadows;

//   Fog cfog;

//   void (*glptrActiveTexture)( GLenum );
//   void (*glptrMultiTexCoord2f)( GLenum, fpnum, fpnum );
//   void (*glptrMultiTexCoord3f)( GLenum, fpnum, fpnum, fpnum );
//   void (*glptrFogCoordf)( fpnum );

//   GLuint nmname, emname, spname;
//   bool& exists( void ) const{ static bool e; return e; } 
//   static View& vw( void ){ static View v; return v; }
//   word bpp;
// };
Display::Display( word w, word h, const char* nm ) : light( 0, 0, 0 ),
						     shadowcolor( 0, 0, 0, 0.25 ), 
						     anisotropic( true ),
						     trns( new Vector[ 1 ] ), 
						     lght( new Vector[ 1 ] ),
						     hav( new Vector[ 1 ] ),
						     fogdepths( new fpnum[ 1 ] ),
						     visibility( new bool[ 1 ] ),
						     tsz( 1 ), vissize( 1 ),
						     shadowverts( new Vector[ 256 ] ),
						     numshadows( 0 ), maxshadows( 256 ){
  
  vw().position = Vector( 0, 0, 1 );
  vw().viewport.low( Point( 0, 0 ) );
  vw().viewport.high( Point( w, h ) );
  if( exists() )
    throw vis_error( "Attempt to double instantiate the display" );
  exists() = true;
  if( SDL_Init( SDL_INIT_VIDEO ) ){
    char em[ 200 ];
    char* beg = "Failed to initialize SDL with this error: ";
    char* sdle = SDL_GetError();
    if( ( strlen( beg ) + strlen( sdle ) ) > 199 )
      strcpy( em, "Failed to initialize SDL." );
    else{
      strcpy( em, beg );
      strcat( em, sdle );
    }
    throw vis_environment_error( em );
  }
  
  bool gotit = false;

  bpp = 32;
  SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
  SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 32 );
  SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
  gotit = SDL_SetVideoMode( w, h, 32, SDL_OPENGL );
  if( !gotit ){
    bpp = 24;
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 8 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 24 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    gotit = SDL_SetVideoMode( w, h, 24, SDL_OPENGL );
  }
  if( !gotit ){
    bpp = 16;
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 16 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    gotit = SDL_SetVideoMode( w, h, 16, SDL_OPENGL );
  }
  if( !gotit ){
    bpp = 15;
    SDL_GL_SetAttribute( SDL_GL_RED_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_GREEN_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_BLUE_SIZE, 5 );
    SDL_GL_SetAttribute( SDL_GL_DEPTH_SIZE, 15 );
    SDL_GL_SetAttribute( SDL_GL_DOUBLEBUFFER, 1 );
    gotit = SDL_SetVideoMode( w, h, 15, SDL_OPENGL );
  }
  if( !gotit ){
    char em[ 200 ];
    char* beg = "Failed to initialize SDL with this error: ";
    char* sdle = SDL_GetError();
    if( ( strlen( beg ) + strlen( sdle ) ) > 199 )
      strcpy( em, "Failed to initialize SDL." );
    else{
      strcpy( em, beg );
      strcat( em, sdle );
    }
    throw vis_environment_error( em );
  }
  
  // Get opengl proc addresses
  glptrActiveTexture = ( void (*)( GLenum ) )SDL_GL_GetProcAddress( "glActiveTexture" );
  if( glptrActiveTexture == NULL )
    throw vis_environment_error( "Failed to get proccess address for glActiveTexture" );

  glptrMultiTexCoord3f = ( void (*)( GLenum, fpnum, fpnum, fpnum ) )SDL_GL_GetProcAddress( "glptrMultiTexCoord3f" );
  if( glptrMultiTexCoord3f == NULL )
    throw vis_environment_error( "Failed to get proccess address for glptrMultiTexCoord3f" );
  glptrMultiTexCoord2f = ( void (*)( GLenum, fpnum, fpnum ) )SDL_GL_GetProcAddress( "glptrMultiTexCoord2f" );
  if( glptrMultiTexCoord2f == NULL )
    throw vis_environment_error( "Failed to get proccess address for glptrMultiTexCoord2f" );

  glptrFogCoordf = ( void (*)( fpnum ) )SDL_GL_GetProcAddress( "glFogCoordf" );
  if( glptrFogCoordf == NULL )
    throw vis_environment_error( "Failed to get proccess address for glptrFogCoordfARB" );

  // Hide mouse
  SDL_ShowCursor( SDL_DISABLE );
  SDL_WarpMouse( w / 2, h / 2 );

  // Set title
  SDL_WM_SetCaption( nm, nm );

  // Check for at least three texture units( rqr'd for bump mapping )
  if( GL_MAX_TEXTURE_UNITS < 4 )
    throw vis_environment_error( "Three OpenGL texture units not supported" );

  // do openGL init
#ifdef DBG
  cout << "Initing GL with a " << w << "x" << h << " window" << endl;
#endif
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glViewport( 0, 0, w, h ); 

  glShadeModel( GL_SMOOTH );
  glClearColor( 0, 0, 0, 0 );	
  
  glClearDepth( 1.0f );		
  glEnable( GL_DEPTH_TEST );
  glDepthFunc( GL_LEQUAL );
  
  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

  glFrontFace( GL_CCW );
  glCullFace( GL_BACK );
  glEnable( GL_CULL_FACE );

  glEnable( GL_LIGHTING );
  glLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, 1 );
  glLightModeli( GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR );

  GLenum err = glGetError();
  if( err != GL_NO_ERROR )
    throw vis_environment_error( "Failed to initialize OpenGL lighting" );

  // Tex0 is normal cube map.
  glptrActiveTexture( GL_TEXTURE0 );
  glEnable( GL_TEXTURE_CUBE_MAP );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
  loadnormalmap();
  // Tex1 is bumpmap.
  glptrActiveTexture( GL_TEXTURE1 );
  glEnable( GL_TEXTURE_2D );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
  glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_DOT3_RGB );
  // Tex2 is diffuse.
  glptrActiveTexture( GL_TEXTURE2 );
  glEnable( GL_TEXTURE_2D );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
  glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
  // Tex3 is emission.
  glptrActiveTexture( GL_TEXTURE3 );
  glEnable( GL_TEXTURE_2D );
  glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE );
  glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD );
   
  glEnable( GL_BLEND );
  glFogi( GL_FOG_COORDINATE_SOURCE, GL_FOG_COORDINATE );

  //  Load default textures
  static RGBSurface bumptex( 1, 1 );
  static RGBSurface diffusetex( 1, 1 );
  static RGBSurface emissiontex( 1, 1 );
  static RGBSurface speculartex( 1, 1 );
  bumptex.setpixel( Point( 0, 0 ), Pixel( 127, 127, 255 ) );
  diffusetex.setpixel( Point( 0, 0 ), Pixel( 255, 255, 255 ) );
  emissiontex.setpixel( Point( 0, 0 ), Pixel( 0, 0, 0 ) );
  speculartex.setpixel( Point( 0, 0 ), Pixel( 96, 96, 96 ) );
  bumpmap( bumptex );
  texture( diffusetex );
  emissionmap( emissiontex );
  specularmap( speculartex );

  if( err != GL_NO_ERROR )
    throw vis_environment_error( "Failed to initialize OpenGL textures" );

}
Display::~Display( void ){
  delete[] trns;
  delete[] plght;
  delete[] lght;
  delete[] hav;
  delete[] fogdepths;
  delete[] visibility;
  delete[] shadowverts;
  glDeleteTextures( 1, &nmname );
  SDL_Quit();
  exists() = false;
}
void Display::draw( Drawable& d ){
  if( d.vertexcount() > tsz ){
    delete[] trns;
    delete[] plght;
    delete[] lght;
    delete[] hav;
    delete[] fogdepths;
    trns = new Vector[ d.vertexcount() ];
    plght = new Vector[ d.vertexcount() ];
    lght = new Vector[ d.vertexcount() ];
    hav = new Vector[ d.vertexcount() ];
    fogdepths = new fpnum[ d.vertexcount() ];
    tsz = d.vertexcount();
  }
  if( d.facecount() > vissize ){
    vissize = d.facecount();
    delete[] visibility;
    visibility = new bool[ vissize ];
  }

#ifdef DBG
  cout << "Display::drawing" << endl;
#endif
  glMatrixMode( GL_MODELVIEW );
  glLoadIdentity();
  glTranslatef( -vw().position.x, -vw().position.y, -vw().position.z );

  Matrix rt;
  rt.rotate( d.rotation );
  Matrix ps( rt );
  rt.identity();
  rt.unrotate( d.rotation );
  ps.translate( d.position );
  // trns are the transformed position, plght and lght are the light vectors, 
  // before and after unrotating into object space, and hav are the half angle vectors, the later
  for( size_t i = 0; i < d.vertexcount(); i++ ){
    trns[ i ] = d.hull->vertices[ i ];
    trns[ i ].transform( ps );
    lght[ i ] = light;
    lght[ i ] -= trns[ i ];
    lght[ i ].normal();
    plght[ i ] = lght[ i ];
    lght[ i ].transform( rt );
  }
  if( d.specular || ( cfog.type != Fog::none ) ){
    for( size_t i = 0; i < d.vertexcount(); i++ ){
      hav[ i ] = vw().position;
      hav[ i ] -= trns[ i ];
    }
  }
  if( cfog.type != Fog::none )
    for( size_t i = 0; i < d.vertexcount(); i++ )
      fogdepths[ i ] = hav[ i ].length();
  if( d.specular ){
    for( size_t i = 0; i < d.vertexcount(); i++ ){
      hav[ i ].normal();
      hav[ i ].transform( rt );
      hav[ i ] += lght[ i ];
      hav[ i ].normal();
    }
  }

  // set up for diffuse
  glBlendFunc( GL_ONE, GL_ZERO );
  glptrActiveTexture( GL_TEXTURE2 );
  glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_TEXTURE2 );
  glptrActiveTexture( GL_TEXTURE3 );
  glBindTexture( GL_TEXTURE_2D, emname );
  glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_ADD );

  glBegin( GL_TRIANGLES );
  // Do diffuse and emisive.
  for( size_t i = 0; i < d.facecount(); i++ ){
    for( size_t j = 0; j < 3; j++ ){
      size_t texindex = d.textures[ i ][ j ];
      size_t index = d.hull->faces[ i ][ j ];
      fpnum s =	d.texcoords[ texindex ].x;
      fpnum t = d.texcoords[ texindex ].y;
      // BUGBUG fix normal map so that we don't have to invert the lght vector here.
      glptrMultiTexCoord3f( GL_TEXTURE0,
			 -lght[ index ].dot( d.texcoords[ texindex ].t ), 
			 lght[ index ].dot( d.texcoords[ texindex ].s ),
			 lght[ index ].dot( d.texcoords[ texindex ].r ) );
      glptrMultiTexCoord2f( GL_TEXTURE1, s, t ); 
      glptrMultiTexCoord2f( GL_TEXTURE2, s, t );
      glptrMultiTexCoord2f( GL_TEXTURE3, s, t ); 
      if( cfog.type != Fog::none )
	glptrFogCoordf( fogdepths[ index ] );
      glVertex3f( trns[ index ].x,
		  trns[ index ].y,
		  trns[ index ].z );
      
    }
  } 
  glEnd();

  // set up for specular.
  if( d.specular ){
    glBlendFunc( GL_SRC_COLOR, GL_ONE );
    glBindTexture( GL_TEXTURE_2D, spname );
    glTexEnvi( GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE );
    glptrActiveTexture( GL_TEXTURE2 );
    glTexEnvi( GL_TEXTURE_ENV, GL_SOURCE0_RGB, GL_PREVIOUS );
    
    glBegin( GL_TRIANGLES );
    for( size_t i = 0; i < d.facecount(); i++ ){
      for( size_t j = 0; j < 3; j++ ){
	size_t texindex = d.textures[ i ][ j ];
	size_t index = d.hull->faces[ i ][ j ];
	fpnum s = d.texcoords[ texindex ].x;
	fpnum t = d.texcoords[ texindex ].y;
	// BUGBUG fix normal map so that we don't have to invert the lght vector here.
	glptrMultiTexCoord3f( GL_TEXTURE0,
			   -hav[ index ].dot( d.texcoords[ texindex ].t ), 
			   hav[ index ].dot( d.texcoords[ texindex ].s ),
			   hav[ index ].dot( d.texcoords[ texindex ].r ) );
	glptrMultiTexCoord2f( GL_TEXTURE1, s, t ); 
	glptrMultiTexCoord2f( GL_TEXTURE2, s, t );
	glptrMultiTexCoord2f( GL_TEXTURE3, s, t ); 
	if( cfog.type != Fog::none )
	  glptrFogCoordf( fogdepths[ index ] );
	glVertex3f( trns[ index ].x,
		    trns[ index ].y,
		    trns[ index ].z );
	
      }
    } 
    glEnd();
  }

  // do shadowing if shadowdist > 0;
  if( ( d.shadowdepth > 0 ) && ( shadowcolor.a > 0 ) ){
    // Find faces that the light shines on and set visibility.
    for( size_t i = 0; i < d.facecount(); i++ )
      if( lght[ d.hull->faces[ i ][ 0 ] ].dot( d.hull->normals[ i ] ) > 0 )
	visibility[ i ] = true;
      else
	visibility[ i ] = false;
    
    // Now push the vertices into shadowverts
    for( size_t i = 0; i < d.facecount(); i++ ){
      // Look at each neighbor if this face is visible
      if( visibility[ i ] ){
	for( size_t j = 0; j < 3; j++ ){
	  //check to see if this has a neighbor that isn't visible.
	  if( !visibility[ d.neighbors[ i ][ j ] ] || 
	      ( d.neighbors[ i ][ j ] == i ) ){
	    // Push the four corners onto shadowverts
	    // Fixme not to use stack.
	    if( numshadows + 4 >= maxshadows ){
	      Vector* newstack = new Vector[ maxshadows * 2 ];
	      maxshadows *= 2;
	      for( size_t i = 0; i < numshadows; i++ )
		newstack[ i ] = shadowverts[ i ];
	      delete[] shadowverts;
	      shadowverts = newstack;
	    }
	    size_t index = d.hull->faces[ i ][ j ];
 	    shadowverts[ numshadows++ ] = trns[ index ];
 	    shadowverts[ numshadows ] = plght[ index ];
	    shadowverts[ numshadows ] *= -d.shadowdepth;
	    shadowverts[ numshadows++ ] += trns[ index ];
 	    index = d.hull->faces[ i ][ ( j + 1 ) % 3 ];
 	    shadowverts[ numshadows++ ] = trns[ index ];
 	    shadowverts[ numshadows ] = plght[ index ];
 	    shadowverts[ numshadows ] *= -d.shadowdepth;
	    shadowverts[ numshadows++ ] += trns[ index ];
	  }
	}
      }
    }
  }
}
void Display::texture( const RGBSurface& srf ){
  glptrActiveTexture( GL_TEXTURE2 );
  glBindTexture( GL_TEXTURE_2D, srf.glnm );
  if( srf.dirty )
    loadgltex( srf );
}
void Display::bumpmap( const RGBSurface& srf ){
  glptrActiveTexture( GL_TEXTURE1 );
  glBindTexture( GL_TEXTURE_2D, srf.glnm );
  if( srf.dirty )
    loadgltex( srf );
}
void Display::emissionmap( const RGBSurface& srf ){
  emname = srf.glnm;
  if( srf.dirty ){
    glptrActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, srf.glnm );
    loadgltex( srf );
  }
}
void Display::specularmap( const RGBSurface& srf ){
  spname = srf.glnm;
  if( srf.dirty ){
    glptrActiveTexture( GL_TEXTURE3 );
    glBindTexture( GL_TEXTURE_2D, srf.glnm );
    loadgltex( srf );
  }
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

  if( ( shadowcolor.a > 0 ) && numshadows ){
    //Draw all the shadows on the stack. Disable texturing and lighting
    glptrActiveTexture( GL_TEXTURE3 );
    glDisable( GL_TEXTURE_2D );
    glptrActiveTexture( GL_TEXTURE2 );
    glDisable( GL_TEXTURE_2D );
    glptrActiveTexture( GL_TEXTURE1 );
    glDisable( GL_TEXTURE_2D );
    glptrActiveTexture( GL_TEXTURE0 );
    glDisable( GL_TEXTURE_CUBE_MAP );
    glDisable( GL_LIGHTING );

    // Set up for stencil passes.
    glDepthMask( GL_FALSE );
    glColorMask( GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE );
    glEnable( GL_STENCIL_TEST );
    glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
  
    // Turn "on" the shadows.
    glStencilFunc( GL_ALWAYS, 0, 0xffffffff );
    glStencilOp( GL_KEEP, GL_KEEP, GL_INCR );
    for( size_t i = 0; i < numshadows; i += 4 ){
      glBegin( GL_TRIANGLE_STRIP );
      glVertex3f( shadowverts[ i + 0 ].x, shadowverts[ i + 0 ].y, shadowverts[ i + 0 ].z );
      glVertex3f( shadowverts[ i + 1 ].x, shadowverts[ i + 1 ].y, shadowverts[ i + 1 ].z );
      glVertex3f( shadowverts[ i + 2 ].x, shadowverts[ i + 2 ].y, shadowverts[ i + 2 ].z );
      glVertex3f( shadowverts[ i + 3 ].x, shadowverts[ i + 3 ].y, shadowverts[ i + 3 ].z );
      glEnd();
    }

    // Turn "off" the backside of the shadows
    glStencilOp( GL_KEEP, GL_KEEP, GL_DECR );
    glFrontFace( GL_CW );
    for( size_t i = 0; i < numshadows; i += 4 ){
      glBegin( GL_TRIANGLE_STRIP );
      glVertex3f( shadowverts[ i + 0 ].x, shadowverts[ i + 0 ].y, shadowverts[ i + 0 ].z );
      glVertex3f( shadowverts[ i + 1 ].x, shadowverts[ i + 1 ].y, shadowverts[ i + 1 ].z );
      glVertex3f( shadowverts[ i + 2 ].x, shadowverts[ i + 2 ].y, shadowverts[ i + 2 ].z );
      glVertex3f( shadowverts[ i + 3 ].x, shadowverts[ i + 3 ].y, shadowverts[ i + 3 ].z );
      glEnd();
    }
    numshadows = 0;
    glFrontFace( GL_CCW );

    // Set up for shadow rectangle.
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

    // Enable texturing and lighting
    glptrActiveTexture( GL_TEXTURE3 );
    glEnable( GL_TEXTURE_2D );
    glptrActiveTexture( GL_TEXTURE2 );
    glEnable( GL_TEXTURE_2D );
    glptrActiveTexture( GL_TEXTURE1 );
    glEnable( GL_TEXTURE_2D );
    glptrActiveTexture( GL_TEXTURE0 );
    glEnable( GL_TEXTURE_CUBE_MAP );
    glEnable( GL_LIGHTING );
  
  }
  // glFlush();
  SDL_GL_SwapBuffers();
}
void Display::clear( const Color& clr ){
#ifdef DBG
  cout << "Display::clearing" << endl;
#endif
  glClearColor( clr.r, clr.g, clr.b, 0 );
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );
  glClearColor( 0, 0, 0, 0 );
}
fpnum Display::aspect( void ){
  return (fpnum)vw().viewport.width() / (fpnum)vw().viewport.height();
}
dword Display::width( void ){
  return vw().viewport.width();
}
dword Display::height( void ){
  return vw().viewport.height();
}


void Display::look( void ){

#ifdef DBG
  cout << "Display::looking: width=" << width() << 
    " height=" << height() << endl;
#endif
  glMatrixMode( GL_PROJECTION );
  glLoadIdentity();

  glViewport( 0, 0, vw().viewport.width(), vw().viewport.height() );
  fpnum up = tanf( vw().fov * torad / 2 ) * vw().clipnear;
  fpnum right = up * aspect();
  glFrustum( -right, right, -up, up, vw().clipnear, vw().clipfar );

  glRotatef( -vw().rotation.z, 0, 0, 1 );
  glRotatef( -vw().rotation.x, 1, 0, 0 );
  glRotatef( -vw().rotation.y, 0, 1, 0 );
}
void Display::loadnormalmap( void ){
  SDL_Surface* srf = SDL_CreateRGBSurface( 0, 256, 256, 24, 255,
					   (dword)255 << 8,
					   (dword)255 << 16,
					   0 );

  if( !srf ){
    char em[ 200 ];
    char* beg = "Failed to create normal map with this error: ";
    char* sdle = SDL_GetError();
    if( ( strlen( beg ) + strlen( sdle ) ) > 199 )
      strcpy( em, "Failed to initialize SDL." );
    else{
      strcpy( em, beg );
      strcat( em, sdle );
    }
    throw vis_environment_error( em );
  }


#ifdef DBG
  cout << "Initing texturing for normal map";
#endif

  glGenTextures( 1, &nmname );
  glBindTexture( GL_TEXTURE_2D, nmname ); 

  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); 
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_GENERATE_MIPMAP, GL_TRUE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#ifdef GL_TEXTURE_MAX_ANISOTROPY_EXT
  // Set up anisotropic filtering
  GLfloat f;
  glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f );
  glTexParameterf( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY_EXT, f );
#endif
  for( size_t i = 0; i < 256; i++ ){
    for( size_t j = 0; j < 256; j++ ){
      Vector u( i / 127.5f - 1, ( 255 - j ) / 127.5f - 1, 1 );
      u.normal();
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 ] = (byte)( ( u.x + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 1 ] = (byte)( ( u.y + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 2 ] = (byte)( ( u.z + 1 ) * 127.5f );  
    }
  }
  glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, srf->w, srf->h, 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf->pixels );	
  for( size_t i = 0; i < 256; i++ ){
    for( size_t j = 0; j < 256; j++ ){
      Vector u( i / 127.5f - 1, 1, j / 127.5f - 1 );
      u.normal();
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 ] = (byte)( ( u.x + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 1 ] = (byte)( ( u.y + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 2 ] = (byte)( ( u.z + 1 ) * 127.5f );  
    }
  }
  glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, srf->w, srf->h, 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf->pixels );	
  for( size_t i = 0; i < 256; i++ ){
    for( size_t j = 0; j < 256; j++ ){
      Vector u( i / 127.5f - 1, -1, ( 255 - j ) / 127.5f - 1 );
      u.normal();
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 ] = (byte)( ( u.x + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 1 ] = (byte)( ( u.y + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 2 ] = (byte)( ( u.z + 1 ) * 127.5f );  
    }
  }
  glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, srf->w, srf->h, 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf->pixels );	
  for( size_t i = 0; i < 256; i++ ){
    for( size_t j = 0; j < 256; j++ ){
      Vector u( 1, ( 255 - j ) / 127.5f - 1, ( 255 - i ) / 127.5f - 1 );
      u.normal();
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 ] = (byte)( ( u.x + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 1 ] = (byte)( ( u.y + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 2 ] = (byte)( ( u.z + 1 ) * 127.5f );  
    }
  }
  glTexImage2D( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, srf->w, srf->h, 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf->pixels );	
  for( size_t i = 0; i < 256; i++ ){
    for( size_t j = 0; j < 256; j++ ){
      Vector u( -1, ( 255 - j ) / 127.5f - 1, i / 127.5f - 1 );
      u.normal();
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 ] = (byte)( ( u.x + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 1 ] = (byte)( ( u.y + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 2 ] = (byte)( ( u.z + 1 ) * 127.5f );  
    }
  }
  glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, srf->w, srf->h, 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf->pixels );	
  for( size_t i = 0; i < 256; i++ ){
    for( size_t j = 0; j < 256; j++ ){
      Vector u( ( 255 - i ) / 127.5f - 1, ( 255 - j ) / 127.5f - 1, -1 );
      u.normal();
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 ] = (byte)( ( u.x + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 1 ] = (byte)( ( u.y + 1 ) * 127.5f );  
      ( (byte*)( srf->pixels ) )[ ( i + j * 256 ) * 3 + 2 ] = (byte)( ( u.z + 1 ) * 127.5f );  
    }
  }
  glTexImage2D( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, srf->w, srf->h, 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf->pixels );	

  SDL_FreeSurface( srf );

  GLenum err = glGetError();
  if( err != GL_NO_ERROR )
    throw vis_environment_error( "OpenGL error while loading texture" );

} 
void Display::loadgltex( const RGBSurface& srf ){
  // Const cast away.
  ( (RGBSurface*)( &srf ) )->dirty = false;

  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR ); 
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
  glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
  glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
#ifdef GL_TEXTURE_MAX_ANISOTROPY_EXT
  // Set up anisotropic filtering
  if( anisotropic ){
    GLfloat f;
    glGetFloatv( GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &f );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, f );
  } else
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 1 );
#endif
  glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, srf.width(), srf.height(), 
		0, GL_RGB, GL_UNSIGNED_BYTE, srf.data );	
}
