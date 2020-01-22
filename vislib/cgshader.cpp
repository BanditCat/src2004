//////////////////////////////////////////////////////
// vis v2 drawable source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <cstdio>
#include "vis/assess.H"
#include "vis/vis.H"

using namespace std; 



CGShader::CGShader( const char* filename ) : initialized( false ), plisting( NULL ){
  pfilename = new char[ strlen( filename ) + 1 ];
  strcpy( pfilename, filename );
}
CGShader::~CGShader( void ){
  if( plisting != NULL )
    delete[] plisting;
  delete[] pfilename;
}



void CGShader::bind( void ){
  if( !initialized ){
    initialized = true;
    load( pfilename );
    init();
  }
  if( !enabled() ){
    enabled() = true;
    cgGLEnableProfile( pvertexprofile() );
    cgGLEnableProfile( pfragmentprofile() );
    if( cgcheckerror() )
      cgerror( "Cg error while enabling profiles:\n" );
  }
  cgGLBindProgram( cgprogram );
  if( cgcheckerror() )
    cgerror( "Failed to bind Cg program with this error:\n" );
  pbind();
}
void CGShader::disable( void ){
  cgGLDisableProfile( pvertexprofile() );
  cgGLDisableProfile( pfragmentprofile() );
  enabled() = false;
}



void CGShader::load( const char* filename ){
#ifdef DBG
  cout << "CGShader::loading" << endl;
#endif

  FILE* file = fopen( filename, "r" );
  bool fileexists = false;
  if( file != NULL ){
    fileexists = true;
    fclose( file );
  }
  if( !fileexists ){
    static char emsg[ 1024 ];
    strcpy( emsg, "Failed to load the Cg shader file " );
    strncat( emsg, filename, 1022 - strlen( emsg ) );
    throw vis_environment_error( emsg );
  }   
  
  

  cgprogram = cgCreateProgramFromFile( cgcontext(), CG_SOURCE, filename, 
				       ( type() == vertex ) ? pvertexprofile() : pfragmentprofile(), "main", NULL );
  const char* tlisting = cgGetLastListing( cgcontext() );
  if( cgprogram == NULL ){
    if( ( cgGetError() == CG_COMPILER_ERROR ) && ( tlisting != NULL ) ){
      static char emsg[ 4096 ];
      strcpy( emsg, "Failed to compile Cg shader with this listing:\n" );
      strncat( emsg, tlisting, 4094 - strlen( emsg ) );
      throw vis_environment_error( emsg );
    } else
      cgerror( "Failed to compile Cg shader with this error:\n" );
    }
    if( tlisting == NULL )
      cgerror( "Failed to get Cg shader compililation listing with this error:\n" );
    plisting = new char[ strlen( tlisting ) + 1 ];
    strcpy( plisting, tlisting );

    if( cgcheckerror() )
      cgerror( "Failed to load Cg shader with this error:\n" );

    cgGLLoadProgram( cgprogram );
    if( cgcheckerror() )
      cgerror( "Failed to load Cg program with this error:\n" );

}
void CGShader::cgerror( const char* error ){
  CGerror err = cgGetError();

  static char emsg[ 1024 ];
  if( strlen( error ) + strlen( cgGetErrorString( err ) ) + 1 >= 1024 )
    throw vis_environment_error( "An unspecified cg error has occured" );
  strcpy( emsg, error );
  strcat( emsg, cgGetErrorString( err ) );
  throw vis_environment_error( emsg );
}
bool CGShader::cgcheckerror( void ){
  return cgGetError();
}
void CGShader::cginit( void ){
  cgcontext() = cgCreateContext();

  if( cgcontext == NULL )
    cgerror( "Cg context creation failed with this error: " );
  
  pvertexprofile() = cgGLGetLatestProfile( CG_GL_VERTEX );
  cgGLSetOptimalOptions( pvertexprofile() );
  pfragmentprofile() = cgGLGetLatestProfile( CG_GL_FRAGMENT );
  cgGLSetOptimalOptions( pfragmentprofile() );
  if( cgcheckerror() )
    cgerror( "Cg error while creating profiles:\n" );
}
void CGShader::cgclose( void ){
  cgGLDisableProfile( pvertexprofile() );
  cgGLDisableProfile( pfragmentprofile() );
  if( cgcheckerror() )
    cgerror( "Cg error while disabling profiles:\n" );
}

