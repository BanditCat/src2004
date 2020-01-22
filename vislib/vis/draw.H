////////////////////////////////////////////////
// Visualization v2 drawable header.
// (c) Jon DuBois 2004
// 10/23/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef DRAW_H
#define DRAW_H
#include <iostream>
#include "vis/vis.H"
#include <GL/gl.h>



// Declerations.
class Drawable;
/////////////////////////////////////////////
// Faces
//
// These are indices into arrays that define 
// a face, if you are looking down at the
// front of the face, the vertices are always
// going counter-clockwise in ascending order.
/////////////////////////////////////////////
struct Face{
  size_t indices[ 3 ];
  size_t& operator[]( size_t ind ){ return indices[ ind ]; }
};



/////////////////////////////////////////////
// Geometries
//
// This defines a geometry as a triangular 
// mesh. Normals are per face, not per vertex.
/////////////////////////////////////////////
struct Geometry{
  Geometry( size_t nfaces, size_t nverts ) : faces( new Face[ nfaces ] ),
					     normals( new Vector[ nfaces ] ),
					     vertices( new Vector[ nverts ] ),
					     vsize( nverts ), fsize( nfaces ){}
  ~Geometry( void ){ delete[] faces; delete[] vertices; delete[] normals; }

  Face* faces;
  Vector* normals;
  Vector* vertices;

  size_t facecount( void ){ return fsize; }
  size_t vertexcount( void ){ return vsize; }
  void findnormals( void );
private:
  size_t vsize, fsize;
};



////////////////////////////////////////////
// Texcoords
// 
// This defines a texture coordinate along
// with a texture coordinate system (which
// defines x, y and z from the point of
// view of the bitmap)
////////////////////////////////////////////
struct Texcoord{
  fpnum x, y;
  // s, t and r define texture space.
  Vector s, t, r;
};



/////////////////////////////////////////////
// Renders.
//
//
// These encapsulate the information required to 
// Render a drawable.
/////////////////////////////////////////////
struct Render{
  Drawable* object;
  // if zero, shadows don't get drawn.
  fpnum shadowdepth;
  // if zero, there is no specular.
  fpnum specular;
  Matrix transform;
  // if NULL, texture doesn't get changed.
  Texture* texture;
  Texture* bumpmap;
  Texture* specularmap;
};



/////////////////////////////////////////////
// Drawables
//
// And finally the actual thing that gets
// drawn. textures defines the offsets into 
// texcoords of the faces. Neighbors contains
// facial indices, where the first datum is
// the neighbor along the first edge etc. 
// Hull is the geometry of the object.
/////////////////////////////////////////////
class Drawable{
public:
  Drawable( void );
  virtual ~Drawable( void ) = 0;

  Geometry* hull;
  Texcoord* texcoords;
  Face* textures;
  Face* neighbors;

  size_t facecount( void ) const{ return numfaces; }
  size_t vertexcount( void ) const{ return numvertices; }
  size_t texturecount( void ) const{ return numtexcoords; }

  // Compile compiles the drawable using ARB_vertex_buffer_object, compiled checks
  // to see if this drawable has been successfully compiled. This depends on vertices not
  // sharing texcoords, as in there is a seperate texcoord for everey vertex, but not
  // neccesarily the other way around.
  bool compile( void );
  bool compiled( void ) const{ return pcompiled; }
  friend class Display;

  // This is a devolopment tool; it tests whether neighbors contains correct data.
  bool testneighbors( void );
protected:
  size_t numfaces, numvertices, numtexcoords;
  // This fills in the s, t and r values of texcoords, as well as the normals of the hull.
  // if smooth is set, then all texcoords of the same vertex will have the same texture space.
  void findtexturespace( bool smooth );
  // This fills in the neighbor values of faces.
  void findneighbors( void );
private:
  bool pcompiled;
  GLuint vertexbuffer, sbuffer, tbuffer, xybuffer, indexbuffer;
};



class Grid : public Drawable{
public:
  Grid( size_t w, size_t h );
  ~Grid( void );

  size_t width( void ){ return wdth; }
  size_t height( void ){ return hght; }
  void set( size_t, size_t, fpnum );
  
  void ripple( fpnum phase );
  
private:
  size_t wdth, hght;
};



class Torus : public Drawable{
public:
  Torus( fpnum outerrad, fpnum innerrad, size_t outersegs, size_t innersegs );
  ~Torus( void );

  void set( fpnum outerrad, fpnum innerrad );
private:
  size_t inr, otr;
};



class Rectangloid : public Drawable{
public:
  Rectangloid( fpnum width, fpnum height, fpnum depth, bool inside );
  ~Rectangloid( void );

  void set( fpnum width, fpnum height, fpnum depth, bool inside );
};



#endif //DRAW_H
