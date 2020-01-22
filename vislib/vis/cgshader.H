////////////////////////////////////////////////
// Visualization v2 shader header.
// (c) Jon DuBois 2004
// 10/23/2004
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef CGSHADER_H
#define CGSHADER_H
#include <iostream>
#include "vis/vis.H"
#include <GL/gl.h>
#include <Cg/cg.h>
#include <Cg/cgGL.h>



/////////////////////////////////////////////
// CGShader
//
// This is the parrent class for all shaders..
/////////////////////////////////////////////
class CGShader{
public:
  CGShader( const char* );
  virtual ~CGShader( void );

  void bind( void );
  static void disable( void );

  const char* listing( void ) const{ return plisting; }

  static const char* vertexprofile( void ){ return cgGetProfileString( pvertexprofile() ); }
  static const char* fragmentprofile( void ){ return cgGetProfileString( pfragmentprofile() ); }

  friend class Display;
protected:

  CGprogram cgprogram;

  // This appends the cg error description to the suplied argument and throws an exception.
  static void cgerror( const char* );
  // This returns true if the Cg subsystem is reporting an error.
  static bool cgcheckerror( void );
  virtual enum Type { vertex, fragment } type( void ) = 0;

private:
  void load( const char* );

  virtual void init( void ) = 0;
  virtual void pbind( void ) = 0;

  static void cginit( void );
  static void cgclose( void );

  bool initialized;
  static bool& enabled( void ){ static bool e = false; return e; }
  char *plisting, *pfilename;
  static CGcontext& cgcontext( void ){ static CGcontext c; return c; }
  static CGprofile& pvertexprofile( void ){ static CGprofile p; return p; }
  static CGprofile& pfragmentprofile( void ){ static CGprofile p; return p; }
};



#endif //CGSHADER_H
