//////////////////////////////////////////////////////
// vis v2 drawable source file.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <cmath>
#include "vis/assess.H"
#include "vis/vis.H"

using namespace std; 



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




Drawable::Drawable( void ) : hull( NULL ), texcoords( NULL ), textures( NULL ), 
			      neighbors( NULL ), pcompiled( false ){}
Drawable::~Drawable( void ){
  if( pcompiled ){
    Display::glptrDeleteBuffers( 1, &vertexbuffer );
  }
}



bool Drawable::compile( void ){
  if( !Display::canusebuffers )
    return false;

  GLint prevbinding;
  glGetIntegerv( GL_ARRAY_BUFFER_BINDING_ARB, &prevbinding );

  Display::glptrGenBuffers( 1, &vertexbuffer );
  Display::glptrBindBuffer( GL_ARRAY_BUFFER_ARB, vertexbuffer );
  size_t dssize = numtexcoords * 3;
  GLfloat* datastore = new GLfloat[ dssize ];
  for( size_t i = 0; i < dssize; i++ )
    datastore[ i ] = 0;

  for( size_t i = 0; i < numfaces; i++ ){
    for( size_t j = 0; j < 3; j++ ){
      size_t texindex = textures[ i ][ j ];
      size_t vertindex = hull->faces[ i ][ j ];
      if( ( ( datastore[ texindex * 3 ] != 0 ) && 
	    ( datastore[ texindex * 3 ] != hull->vertices[ vertindex ].x ) ) ||
	  ( ( datastore[ texindex * 3 + 1 ] != 0 ) && 
	    ( datastore[ texindex * 3 + 1 ] != hull->vertices[ vertindex ].y ) ) ||
	  ( ( datastore[ texindex * 3 + 2 ] != 0 ) && 
	    ( datastore[ texindex * 3 + 2 ] != hull->vertices[ vertindex ].z ) ) )
	throw vis_error( "Attempt to compile a Drawable with a one-texcoord-to-many-vertices correspondence" );
      datastore[ texindex * 3 ] = hull->vertices[ vertindex ].x;
      datastore[ texindex * 3 + 1 ] = hull->vertices[ vertindex ].y;
      datastore[ texindex * 3 + 2 ] = hull->vertices[ vertindex ].z;
    }
  }
  Display::glptrBufferData( GL_ARRAY_BUFFER_ARB, dssize * sizeof( GLfloat ), datastore, GL_STATIC_DRAW_ARB );

  Display::glptrGenBuffers( 1, &sbuffer );
  Display::glptrBindBuffer( GL_ARRAY_BUFFER_ARB, sbuffer );
  {
    size_t j = 0;
    for( size_t i = 0; i < numtexcoords; i++ ){
      datastore[ j++ ] = texcoords[ i ].s.x;
      datastore[ j++ ] = texcoords[ i ].s.y;
      datastore[ j++ ] = texcoords[ i ].s.z;
    }
  }
  Display::glptrBufferData( GL_ARRAY_BUFFER_ARB, dssize * sizeof( GLfloat ), datastore, GL_STATIC_DRAW_ARB );

  Display::glptrGenBuffers( 1, &tbuffer );
  Display::glptrBindBuffer( GL_ARRAY_BUFFER_ARB, tbuffer );
  {
    size_t j = 0;
    for( size_t i = 0; i < numtexcoords; i++ ){
      datastore[ j++ ] = texcoords[ i ].t.x;
      datastore[ j++ ] = texcoords[ i ].t.y;
      datastore[ j++ ] = texcoords[ i ].t.z;
    }
  }
  Display::glptrBufferData( GL_ARRAY_BUFFER_ARB, dssize * sizeof( GLfloat ), datastore, GL_STATIC_DRAW_ARB );

  Display::glptrGenBuffers( 1, &xybuffer );
  Display::glptrBindBuffer( GL_ARRAY_BUFFER_ARB, xybuffer );
  {
    size_t j = 0;
    for( size_t i = 0; i < numtexcoords; i++ ){
      datastore[ j++ ] = texcoords[ i ].x;
      datastore[ j++ ] = texcoords[ i ].y;
    }
  }
  Display::glptrBufferData( GL_ARRAY_BUFFER_ARB, numtexcoords * 2 * sizeof( GLfloat ), datastore, GL_STATIC_DRAW_ARB );

  GLuint* idatastore = new GLuint[ numfaces * 3 ];
  Display::glptrGenBuffers( 1, &indexbuffer );
  Display::glptrBindBuffer( GL_ELEMENT_ARRAY_BUFFER_ARB, indexbuffer );
  {
    size_t k = 0;
    for( size_t i = 0; i < numfaces; i++ ){
      for( size_t j = 0; j < 3; j++ )
	idatastore[ k++ ] = textures[ i ][ j ];
    }
  }
  Display::glptrBufferData( GL_ELEMENT_ARRAY_BUFFER_ARB, numfaces * 3 * sizeof( GLuint ), 
			    idatastore, GL_STATIC_DRAW_ARB );
  delete[] idatastore;

  delete[] datastore;
  Display::glptrBindBuffer( GL_ARRAY_BUFFER_ARB, prevbinding );
  Display::checkglerror( "Failed to compile a Drawable with this error : " );

  pcompiled = true;
  return true;
}



void Drawable::findtexturespace( bool smooth ){
  //find hull normals.
  hull->findnormals();
  static Vector zero( 0, 0, 0 );
  // Zero the vectors
  for( size_t i = 0; i < numtexcoords; i++ ){
    texcoords[ i ].s = zero;
    texcoords[ i ].t = zero;
    texcoords[ i ].r = zero;
  }

  // These are to make the texture space per vertex instead of per texcoord.
  Vector *spos, *tpos, *rpos;
  if( smooth ){
    spos = new Vector[ numvertices ];
    tpos = new Vector[ numvertices ];
    rpos = new Vector[ numvertices ];
    for( size_t i = 0; i < numvertices; i++ ){
      spos[ i ] = zero;
      tpos[ i ] = zero;
      rpos[ i ] = zero;
    }
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
    if( smooth ){
      for( size_t j = 0; j < 3; j++ ){
	spos[ hull->faces[ i ][ j ] ] += s0;
	tpos[ hull->faces[ i ][ j ] ] += t0;
	rpos[ hull->faces[ i ][ j ] ] += hull->normals[ i ];
      }
    } else{
      for( size_t j = 0; j < 3; j++ ){
	texcoords[ textures[ i ][ j ] ].s += s0;
	texcoords[ textures[ i ][ j ] ].t += t0;
	texcoords[ textures[ i ][ j ] ].r += hull->normals[ i ];
      }
    }
  }

  if( smooth ){
    for( size_t i = 0; i < numfaces; i++ ){
      for( size_t j = 0; j < 3; j++ ){
	texcoords[ textures[ i ][ j ] ].s = spos[ hull->faces[ i ][ j ] ];
	texcoords[ textures[ i ][ j ] ].t = tpos[ hull->faces[ i ][ j ] ];
	texcoords[ textures[ i ][ j ] ].r = rpos[ hull->faces[ i ][ j ] ];
      }
    }
    delete[] spos;
    delete[] tpos;
    delete[] rpos;
  }

  // Normalize
  for( size_t i = 0; i < numtexcoords; i++ ){
    texcoords[ i ].s.normal();
    texcoords[ i ].t.normal();
    texcoords[ i ].r.normal();
  }

  // Adjust for real tangent space( above algorithim is flawed ).
  for( size_t i = 0; i < numtexcoords; i++ ){
    Vector t = texcoords[ i ].t;
    t *= -1;
    texcoords[ i ].t = texcoords[ i ].s;
    texcoords[ i ].s = t;
  }
}
bool Drawable::testneighbors( void ){
  Face* oldneighbors;
  oldneighbors = new Face[ numfaces ];

  for( size_t i = 0; i < numfaces; i++ )
    for( size_t j = 0; j < 3; j++ )
      oldneighbors[ i ][ j ] = neighbors[ i ][ j ];

  findneighbors();

  bool result = true;
  for( size_t i = 0; i < numfaces; i++ )
    for( size_t j = 0; j < 3; j++ )
      if( oldneighbors[ i ][ j ] != neighbors[ i ][ j ] )
	result = false;
  
  delete[] oldneighbors;
  return result;
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
  findtexturespace( false );
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
  findtexturespace( false );
}



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
  findtexturespace( true );
}



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
  findtexturespace( false );
}
