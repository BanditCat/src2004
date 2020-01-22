//////////////////////////////////////////////////////
// vis drawable source file.
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
#include <cmath>
#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL/SDL.h>
#include "bcj/assess.H"
#include "vis/vis.H"

using namespace std; 

/////////////////////////////////////////////
// Geometries
/////////////////////////////////////////////
// struct Geometry{
//   Geometry( size_t nfaces, size_t nverts ) : faces( new Face[ nfaces ] ),
//             				     normals( new Vector[ nfaces ] ),
// 					     vertices( new Vector[ nverts ] ),
// 					     vsize( nverts ), fsize( nfaces ){}
//   ~Geometry( void ){ delete[] faces; delete[] vertices; delete[] normals; }

//   Face* faces;
//   Vector* normals;
//   Vector* vertices;

//   size_t numfaces( void ){ return fsize; }
//   size_t numvertices( void ){ return vsize; }
//   void findnormals( void );
// private:
//   size_t vsize, fsize;
// };
void Geometry::findnormals( void ){
  for( size_t i = 0; i < fsize; i++ ){
    Vector side0( vertices[ faces[ i ][ 0 ] ] );
    side0 -= vertices[ faces[ i ][ 1 ] ];
    Vector side1( vertices[ faces[ i ][ 2 ] ] );
    side1 -= vertices[ faces[ i ][ 1 ] ];

    normals[ i ] = side1;
    normals[ i ].cross( side0 );
    normals[ i ].normal();
  }
}



/////////////////////////////////////////////
// Drawables
/////////////////////////////////////////////
// class Drawable{
// public:
//   Drawable( void ) : hull( NULL ), texcoords( NULL ), textures( NULL ), neighbors( NULL ),
// 		     shadowdepth( 0 ), specular( false ){}
//   virtual ~Drawable( void ) = 0;

//   Geometry* hull;
//   Texcoord* texcoords;
//   Face* textures;
//   Face* neighbors;

//   Vector position;
//   Angle rotation;
//   fpnum shadowdepth;
//   bool specular;

//   size_t facecount( void ) const{ return numfaces; }
//   size_t vertexcount( void ) const{ return numvertices; }
//   size_t texturecount( void ) const{ return numtexcoords; }

// protected:
//   size_t numfaces, numvertices, numtexcoords;
//   // This fills in the s, t and r values of texcoords, as well as the normal of the face.
//   void findtexturespace( void );
//   // This fills in the neighbor values of faces.
//   void findneighbors( void );
// };
Drawable::~Drawable( void ){}
void Drawable::findtexturespace( void ){
  //find hull normals.
  hull->findnormals();
  static Vector zero( 0, 0, 0 );
  // Zero the vectors
  for( size_t i = 0; i < numtexcoords; i++ ){
    texcoords[ i ].s = zero;
    texcoords[ i ].t = zero;
    texcoords[ i ].r = zero;
  }
  for( size_t i = 0; i < numfaces; i++ ){
    Vector side0( hull->vertices[ hull->faces[ i ][ 0 ] ] );
    side0 -= hull->vertices[ hull->faces[ i ][ 1 ] ];
    Vector side1( hull->vertices[ hull->faces[ i ][ 2 ] ] );
    side1 -= hull->vertices[ hull->faces[ i ][ 1 ] ];
    // Calulate T
    fpnum du0 = texcoords[ textures[ i ][ 0 ] ].y - texcoords[ textures[ i ][ 1 ] ].y;
    fpnum du1 = texcoords[ textures[ i ][ 2 ] ].y - texcoords[ textures[ i ][ 1 ] ].y;
    Vector t0( side0 );
    Vector t1( side1 );
    t0 *= du1;
    t1 *= du0;
    t0 -= t1;
    t0.normal();
    // Calulate S
    du0 = texcoords[ textures[ i ][ 0 ] ].x - texcoords[ textures[ i ][ 1 ] ].x;
    du1 = texcoords[ textures[ i ][ 2 ] ].x - texcoords[ textures[ i ][ 1 ] ].x;
    Vector s0 = side0;
    Vector s1 = side1;
    s0 *= du1;
    s1 *= du0;
    s0 -= s1;
    s0.normal();
    // Calculate R
    Vector cp( s0 );
    cp.cross( t0 );
    if ( cp.dot( hull->normals[ i ] ) < 0.0f ){
      s0 *= -1;
      t0 *= -1;
    }
    for( size_t j = 0; j < 3; j++ ){
      texcoords[ textures[ i ][ j ] ].s += s0;
      texcoords[ textures[ i ][ j ] ].t += t0;
      texcoords[ textures[ i ][ j ] ].r += hull->normals[ i ];
    }
  }
  // Normalize
  for( size_t i = 0; i < numtexcoords; i++ ){
    texcoords[ i ].s.normal();
    texcoords[ i ].t.normal();
    texcoords[ i ].r.normal();
  }
}
void Drawable::findneighbors( void ){
  // "Zero" the neighbors
  for( size_t i = 0; i < numfaces; i++ ){
    for( size_t j = 0; j < 3; j++ ){
      neighbors[ i ][ j ] = i;
    }
  }
  //for each face
  for( size_t lowerbound = 0; lowerbound < numfaces; lowerbound++ ){
    //check each side
    for( size_t thisside = 0; thisside < 3; thisside++ ){
      //only check if not already found
      if( neighbors[ lowerbound ][ thisside ] == lowerbound ){
	//compare to each face
	for( size_t check = lowerbound + 1; check < numfaces; check++ ){
	  //look for matches
	  for( size_t thatside = 0; thatside < 3; thatside++ ){
	    // if matching
	    if( ( hull->faces[ lowerbound ][ thisside ] == 
		  hull->faces[ check ][ ( thatside + 1 ) % 3 ] ) &&
		( hull->faces[ lowerbound ][ ( thisside + 1 ) % 3 ] == 
		  hull->faces[ check ][ thatside ] ) ){
	      neighbors[ lowerbound ][ thisside ] = check;
	      neighbors[ check ][ thatside ] = lowerbound;
	    }
	  }
	}
      }
    }
  }

}



// class Grid : public Drawable{
// public:
//   Grid( size_t w, size_t h );
//   ~Grid( void );

//   size_t width( void ){ return wdth; }
//   size_t height( void ){ return hght; }
//   void set( size_t, size_t, fpnum );
  
//   void ripple( fpnum phase );
  
// private:
//   size_t wdth, hght;
// };
Grid::Grid( size_t w, size_t h ) : wdth( w ), hght( h ){
  numfaces = ( w - 1 ) * ( h - 1 ) * 2;
  numvertices = w * h;
  numtexcoords = w * h;
  texcoords = new Texcoord[ numtexcoords ];
  textures = new Face[ numfaces ];
  neighbors = new Face[ numfaces ];
  hull = new Geometry( numfaces, numvertices );
  if( ( w < 2 ) || ( h < 2 ) )
    throw vis_error( "Attempt to create grid with a dimension less than 2" );
  for( size_t x = 0; x < w; x++ ){
    for( size_t y = 0; y < h; y++ ){
      fpnum s = (fpnum)x / (fpnum)( w - 1 );
      fpnum t = (fpnum)y / (fpnum)( h - 1 );
      texcoords[ x + y * wdth ].x = s;
      texcoords[ x + y * wdth ].y = t;
      hull->vertices[ x + y * wdth ] = Vector( s * 2 - 1, t * 2 - 1, 0 );
    }
  }    
  size_t ftc = 0;
  for( size_t x = 0; x < ( w - 1 ); x++ ){
    for( size_t y = 0; y < ( h - 1 ); y++ ){
      if( ( x + y ) % 2 == 0 ){
	hull->faces[ ftc ][ 0 ] = textures[ ftc ][ 0 ] = x + y * wdth;
	hull->faces[ ftc ][ 1 ] = textures[ ftc ][ 1 ] = ( x + 1 ) + ( y + 1 ) * wdth;
	hull->faces[ ftc ][ 2 ] = textures[ ftc ][ 2 ] = x + ( y + 1 ) * wdth;
	ftc++;
	hull->faces[ ftc ][ 0 ] = textures[ ftc ][ 0 ] = x + y * wdth;
	hull->faces[ ftc ][ 1 ] = textures[ ftc ][ 1 ] = ( x + 1 ) + y * wdth;
	hull->faces[ ftc ][ 2 ] = textures[ ftc ][ 2 ] = ( x + 1 ) + ( y + 1 ) * wdth;
	ftc++;
      } else{
	hull->faces[ ftc ][ 0 ] = textures[ ftc ][ 0 ] = x + ( y + 1 ) * wdth;
	hull->faces[ ftc ][ 1 ] = textures[ ftc ][ 1 ] = ( x + 1 ) + y * wdth;
	hull->faces[ ftc ][ 2 ] = textures[ ftc ][ 2 ] = ( x + 1 ) + ( y + 1 ) * wdth;
	ftc++;
	hull->faces[ ftc ][ 0 ] = textures[ ftc ][ 0 ] = x + y * wdth;
	hull->faces[ ftc ][ 1 ] = textures[ ftc ][ 1 ] = ( x + 1 ) + y * wdth;
	hull->faces[ ftc ][ 2 ] = textures[ ftc ][ 2 ] = x + ( y + 1 ) * wdth;
	ftc++;
      }
    }
  }
  findtexturespace();
  findneighbors();
}
Grid::~Grid( void ){
  delete hull;
  delete[] texcoords;
  delete[] neighbors;
  delete[] textures;
}
void Grid::ripple( fpnum phs ){
  for( size_t i = 0; i < wdth; i++ ){
    for( size_t j = 0; j < hght; j++ ){
      fpnum x = ( ( (fpnum)i / (fpnum)wdth ) - 0.5 ) * 2;
      fpnum y = ( ( (fpnum)j / (fpnum)hght ) - 0.5 ) * 2;
      hull->vertices[ i + j * wdth ].z = sinf( sqrt( x * x + y * y ) * pi * 4 + phs ) / 8; 
    }
  }    
  findtexturespace();
}
// class Torus : public Drawable{
// public:
//   Torus( fpnum outerrad, fpnum innerrad, size_t outersegs, size_t innersegs );
//   ~Torus( void );

//   void set( fpnum outerrad, fpnum innerrad );
// private:
//   size_t inr, otr;
// };
Torus::Torus( fpnum outerrad, fpnum innerrad, size_t outersegs, size_t innersegs ) : inr( innersegs ),
                                                                                     otr( outersegs ){
  if( ( otr < 3 ) || ( inr < 3 ) )
    throw vis_error( "Attempt to create a torus with less than three segments" );
  numvertices = otr * inr;
  numtexcoords = ( otr + 1 ) * ( inr + 1 );
  numfaces = otr * inr * 2;
  hull = new Geometry( numfaces, numvertices );
  texcoords = new Texcoord[ numtexcoords ];
  textures = new Face[ numfaces ];
  neighbors = new Face[ numfaces ];

  size_t ftc = 0;
  for( size_t x = 0; x < inr; x++ ){  
    for( size_t y = 0; y < otr; y++ ){  
      size_t i = ( x + 1 );
      size_t j = ( y + 1 );
      textures[ ftc ][ 0 ] = x + y * ( inr + 1 );
      hull->faces[ ftc ][ 0 ] = x + y * inr;
      textures[ ftc ][ 1 ] = i + j * ( inr + 1 );
      hull->faces[ ftc ][ 1 ] = ( i % inr ) + ( j % otr ) * inr;
      textures[ ftc ][ 2 ] = x + j * ( inr + 1 );
      hull->faces[ ftc ][ 2 ] = x + ( j % otr ) * inr;
      ftc++;
      textures[ ftc ][ 0 ] = x + y * ( inr + 1 );
      hull->faces[ ftc ][ 0 ] = x + y * inr;
      textures[ ftc ][ 1 ] = i + y * ( inr + 1 );
      hull->faces[ ftc ][ 1 ] = ( i % inr ) + y * inr;
      textures[ ftc ][ 2 ] = i + j * ( inr + 1 );
      hull->faces[ ftc ][ 2 ] = ( i % inr ) + ( j % otr ) * inr;
      ftc++;
    }
  }
  findneighbors();
  set( outerrad, innerrad );
}
Torus::~Torus( void ){
  delete[] textures;
  delete hull;
  delete[] texcoords;
  delete[] neighbors;
}


void Torus::set( fpnum outerrad, fpnum innerrad ){
  for( size_t x = 0; x <= inr; x++ ){
    for( size_t y = 0; y <= otr; y++ ){
      // Texture vars
      fpnum s = (fpnum)x / inr;
      fpnum t = (fpnum)y / otr;
      // Sins etc.
      fpnum ic = cosf( s * pi * 2 );
      fpnum is = sinf( s * pi * 2 );
      fpnum oc = cosf( t * pi * 2 );
      fpnum os = sinf( t * pi * 2 );
      // the position
      fpnum l = outerrad - ic * innerrad;
      fpnum px = oc * l;
      fpnum py = os * l;
      fpnum pz = is * innerrad;
      texcoords[ x + y * ( inr + 1 ) ].x = s;
      texcoords[ x + y * ( inr + 1 ) ].y = t;
      hull->vertices[ ( x % inr ) + ( y % otr ) * inr ] = Vector( px, py, pz );
    }
  }    
  findtexturespace();
}



// class Rectangloid : public Drawable{
// public:
//   Rectangloid( fpnum width, fpnum height, fpnum depth, bool inside );
//   ~Rectangloid( void );

//   void set( fpnum width, fpnum height, fpnum depth, bool inside );
// };
Rectangloid::Rectangloid( fpnum width, fpnum height, fpnum depth, bool inside ){
  numfaces = 12;
  numvertices = 8;
  numtexcoords = 24;
  hull = new Geometry( 12, 8 );
  texcoords = new Texcoord[ 24 ];
  textures = new Face[ 12 ];
  neighbors = new Face[ 12 ];

  // front/back
  hull->faces[ 0 ][ 0 ] = 1; hull->faces[ 0 ][ 1 ] = 2; hull->faces[ 0 ][ 2 ] = 0;
  hull->faces[ 1 ][ 0 ] = 2; hull->faces[ 1 ][ 1 ] = 1; hull->faces[ 1 ][ 2 ] = 3;
  hull->faces[ 2 ][ 0 ] = 6; hull->faces[ 2 ][ 1 ] = 5; hull->faces[ 2 ][ 2 ] = 4;
  hull->faces[ 3 ][ 0 ] = 5; hull->faces[ 3 ][ 1 ] = 6; hull->faces[ 3 ][ 2 ] = 7;
  // left/right
  hull->faces[ 4 ][ 0 ] = 5; hull->faces[ 4 ][ 1 ] = 3; hull->faces[ 4 ][ 2 ] = 1;
  hull->faces[ 5 ][ 0 ] = 3; hull->faces[ 5 ][ 1 ] = 5; hull->faces[ 5 ][ 2 ] = 7;
  hull->faces[ 6 ][ 0 ] = 2; hull->faces[ 6 ][ 1 ] = 4; hull->faces[ 6 ][ 2 ] = 0;
  hull->faces[ 7 ][ 0 ] = 4; hull->faces[ 7 ][ 1 ] = 2; hull->faces[ 7 ][ 2 ] = 6;
  // top/bottom
  hull->faces[ 8 ][ 0 ] = 4; hull->faces[ 8 ][ 1 ] = 1; hull->faces[ 8 ][ 2 ] = 0;
  hull->faces[ 9 ][ 0 ] = 1; hull->faces[ 9 ][ 1 ] = 4; hull->faces[ 9 ][ 2 ] = 5;
  hull->faces[ 10 ][ 0 ] = 3; hull->faces[ 10 ][ 1 ] = 6; hull->faces[ 10 ][ 2 ] = 2;
  hull->faces[ 11 ][ 0 ] = 6; hull->faces[ 11 ][ 1 ] = 3; hull->faces[ 11 ][ 2 ] = 7;
  
  for( size_t i = 0; i < 6; i++ ){
    textures[ i * 2 + 0 ][ 0 ] = i * 4 + 0;
    textures[ i * 2 + 0 ][ 1 ] = i * 4 + 1;
    textures[ i * 2 + 0 ][ 2 ] = i * 4 + 2;

    textures[ i * 2 + 1 ][ 0 ] = i * 4 + 1;
    textures[ i * 2 + 1 ][ 1 ] = i * 4 + 0;
    textures[ i * 2 + 1 ][ 2 ] = i * 4 + 3;

    texcoords[ i * 4 + 0 ].x = 0; texcoords[ i * 4 + 0 ].y = 0;
    texcoords[ i * 4 + 1 ].x = 1; texcoords[ i * 4 + 1 ].y = 1;
    texcoords[ i * 4 + 2 ].x = 0; texcoords[ i * 4 + 2 ].y = 1;
    texcoords[ i * 4 + 3 ].x = 1; texcoords[ i * 4 + 3 ].y = 0;
  }

  set( width, height, depth, inside );
  findneighbors();
}
Rectangloid::~Rectangloid( void ){
  delete hull;
  delete[] textures;
  delete[] neighbors;
  delete[] texcoords;
}
void Rectangloid::set( fpnum width, fpnum height, fpnum depth, bool inside ){
  if( inside ){
    hull->vertices[ 7 ] = Vector(  width / 2,  height / 2,  depth / 2 );
    hull->vertices[ 6 ] = Vector( -width / 2,  height / 2,  depth / 2 );
    hull->vertices[ 5 ] = Vector(  width / 2, -height / 2,  depth / 2 );
    hull->vertices[ 4 ] = Vector( -width / 2, -height / 2,  depth / 2 );
    hull->vertices[ 3 ] = Vector(  width / 2,  height / 2, -depth / 2 );
    hull->vertices[ 2 ] = Vector( -width / 2,  height / 2, -depth / 2 );
    hull->vertices[ 1 ] = Vector(  width / 2, -height / 2, -depth / 2 );
    hull->vertices[ 0 ] = Vector( -width / 2, -height / 2, -depth / 2 );
  } else{
    hull->vertices[ 0 ] = Vector(  width / 2,  height / 2,  depth / 2 );
    hull->vertices[ 1 ] = Vector( -width / 2,  height / 2,  depth / 2 );
    hull->vertices[ 2 ] = Vector(  width / 2, -height / 2,  depth / 2 );
    hull->vertices[ 3 ] = Vector( -width / 2, -height / 2,  depth / 2 );
    hull->vertices[ 4 ] = Vector(  width / 2,  height / 2, -depth / 2 );
    hull->vertices[ 5 ] = Vector( -width / 2,  height / 2, -depth / 2 );
    hull->vertices[ 6 ] = Vector(  width / 2, -height / 2, -depth / 2 );
    hull->vertices[ 7 ] = Vector( -width / 2, -height / 2, -depth / 2 );
  }
  findtexturespace();
}
