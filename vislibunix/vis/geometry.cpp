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
#include <SDL/SDL_image.h>
#include "vis/vis.H"

using namespace std; 



/////////////////////////////////////////////////
// Geometry etc
/////////////////////////////////////////////////
// struct Matrix;
// struct Angle{
//   Angle( void ) : x( 0 ), y( 0 ), z( 0 ){}
//   Angle( fpnum xr, fpnum yr, fpnum zr ) : x( xr ), y( yr ), z( zr ){}
//   ~Angle( void ){}

//   fpnum x;
//   fpnum y;
//   fpnum z;
// };
// struct Vector{
//   Vector( void ) : x( 0 ), y( 0 ), z( 0 ){}
//   // Creates a unit vector pointing in the same direction
//   Vector( const Angle& a );
//   Vector( fpnum xp, fpnum yp, fpnum zp ) : x( xp ), y( yp ), z( zp ){}
//   ~Vector( void ){}

//   fpnum abs( void ) const;
//   fpnum dot( const Vector& ) const;
//   fpnum component( const Vector& ) const;
//   void project( const Vector& );
//   void cross( const Vector& );
//   void normal( void );
//   void rotate( const Angle& ang );
//   void unrotate( const Angle& ang );
//   void transform( const Matrix& mt );

//   Vector& operator*=( fpnum scl );
//   Vector& operator/=( fpnum scl );
//   Vector& operator+=( const Vector& );
//   Vector& operator-=( const Vector& );

//   fpnum x;
//   fpnum y;
//   fpnum z;
// };
inline Vector::Vector( const Angle& ang ){
  x = sinf( ang.y * torad ) * cosf( ang.x * torad );
  z = cosf( ang.y * torad ) * cosf( ang.x * torad );
  y = -sinf( ang.x * torad );
}
inline fpnum Vector::length( void ) const{
  return sqrtf( x * x + y * y + z * z );
}
inline fpnum Vector::dot( const Vector& v ) const{
  return x * v.x + y * v.y + z * v.z;
}
inline fpnum Vector::component( const Vector& v ) const{
  return dot( v ) / v.length();
}
inline void Vector::project( const Vector& v ){
  Vector u = v;
  u *= ( dot( v ) / v.dot( v ) );
  *this = u;
}
inline void Vector::cross( const Vector& v ){
  fpnum xn = y * v.z - z * v.y;
  fpnum yn = z * v.x - x * v.z;
  z = x * v.y - y * v.x;
  x = xn;
  y = yn;
}
inline void Vector::normal( void ){
  if( x || y || z )
    *this /= length();
  else{
    x = 0;
    y = 0;
    z = 0;
  }
}
inline void Vector::rotate( const Angle& ang ){
  fpnum xp, yp, zp;
  xp = z * sinf( ang.y * torad ) + x * cosf( ang.y * torad );
  yp = y;
  zp = z * cosf( ang.y * torad ) - x * sinf( ang.y * torad );
  x = xp;
  y = yp * cosf( ang.x * torad ) - zp * sinf( ang.x * torad );
  z = yp * sinf( ang.x * torad ) + zp * cosf( ang.x * torad );
}
inline void Vector::unrotate( const Angle& ang ){
  fpnum xp, yp, zp;
  xp = x;
  yp = y * cosf( -ang.x * torad ) - z * sinf( -ang.x * torad );
  zp = y * sinf( -ang.x * torad ) + z * cosf( -ang.x * torad );
  x = zp * sinf( -ang.y * torad ) + xp * cosf( -ang.y * torad );
  y = yp;
  z = zp * cosf( -ang.y * torad ) - xp * sinf( -ang.y * torad );
}
inline void Vector::transform( const Matrix& m ){
  fpnum xp = m[ 0 ][ 0 ] * x + m[ 1 ][ 0 ] * y + m[ 2 ][ 0 ] * z + m[ 3 ][ 0 ];
  fpnum yp = m[ 0 ][ 1 ] * x + m[ 1 ][ 1 ] * y + m[ 2 ][ 1 ] * z + m[ 3 ][ 1 ];
  z = m[ 0 ][ 2 ] * x + m[ 1 ][ 2 ] * y + m[ 2 ][ 2 ] * z + m[ 3 ][ 2 ];
  x = xp;
  y = yp;
}


inline Vector& Vector::operator*=( fpnum scl ){
  x *= scl;
  y *= scl;
  z *= scl;
  return *this;
}
inline Vector& Vector::operator/=( fpnum scl ){
  x /= scl;
  y /= scl;
  z /= scl;
  return *this;
}
inline Vector& Vector::operator+=( const Vector& v ){
  x += v.x;
  y += v.y;
  z += v.z;
  return *this;
}
inline Vector& Vector::operator-=( const Vector& v ){
  x -= v.x;
  y -= v.y;
  z -= v.z;
  return *this;
}



// struct Color{
//   Color( void ) : r( 1 ), g( 1 ), b( 1 ){}
//   Color( fpnum rp, fpnum gp, fpnum bp ) : r( rp ), g( gp ), b( bp ){}
//   ~Color( void ){}

//   fpnum r;
//   fpnum g;
//   fpnum b;
// };



// struct Matrix{
//   Matrix( void );
//   inline Matrix::Matrix( fpnum* fp ){

//   void identity( void );
//   void translate( const Vector& );
//   void rotate( const Angle& );
//   void unrotate( const Angle& );
//   void multiply( const Matrix& );

//   const fpnum* operator[]( size_t ind ) const;
//   fpnum* operator[]( size_t ind );
//   friend std::ostream& operator<<( std::ostream&, const Matrix& );

// private:
//   fpnum grid[ 4 ][ 4 ];
// };
inline Matrix::Matrix( void ){
  identity();
}
inline Matrix::Matrix( fpnum* fp ){
  for( size_t i = 0; i < 4; i++ ){
    for( size_t j = 0; j < 4; j++ )
      grid[ i ][ j ] = fp[ i + j * 4 ];
  }
}
inline void Matrix::identity( void ){
  static fpnum tf[ 16 ]    = { 1, 0, 0, 0, 
			       0, 1, 0, 0,
			       0, 0, 1, 0, 
			       0, 0, 0, 1 };
  static Matrix tg( tf );
  for( size_t i = 0; i < 4; i++ ){
    for( size_t j = 0; j < 4; j++ )
      grid[ i ][ j ] = tg[ i ][ j ];
  }
}
inline void Matrix::translate( const Vector& v ){
  static fpnum tf[ 16 ]    = { 1, 0, 0, 0, 
			       0, 1, 0, 0,
			       0, 0, 1, 0, 
			       0, 0, 0, 1 };
  static Matrix tg( tf );
  tg[ 3 ][ 0 ] = v.x;
  tg[ 3 ][ 1 ] = v.y;
  tg[ 3 ][ 2 ] = v.z;

  multiply( tg );
}
inline void Matrix::multiply( const Matrix& m ){
  static fpnum tg[ 4 ][ 4 ];
  for( size_t i = 0; i < 4; i++ ){
    for( size_t j = 0; j < 4; j++ ){
      tg[ i ][ j ] = 
	grid[ i ][ 0 ] * m[ 0 ][ j ] +
	grid[ i ][ 1 ] * m[ 1 ][ j ] +
	grid[ i ][ 2 ] * m[ 2 ][ j ] +
	grid[ i ][ 3 ] * m[ 3 ][ j ];
    }
  }
  for( size_t i = 0; i < 4; i++ ){
    for( size_t j = 0; j < 4; j++ )
      grid[ i ][ j ] = tg[ i ][ j ];
  }
}
inline void Matrix::rotate( const Angle& ang ){
  static fpnum tf[ 16 ]    = { 1, 0, 0, 0, 
			       0, 1, 0, 0,
			       0, 0, 1, 0, 
			       0, 0, 0, 1 };
  static Matrix tgx( tf );
  static Matrix tgy( tf );
  static Matrix tgz( tf );
  fpnum cx = cosf( ang.x * torad );
  fpnum sx = sinf( ang.x * torad );
  tgx[ 1 ][ 1 ] = cx;
  tgx[ 2 ][ 1 ] = -sx;
  tgx[ 1 ][ 2 ] = sx;
  tgx[ 2 ][ 2 ] = cx;

  fpnum cy = cosf( ang.y * torad );
  fpnum sy = sinf( ang.y * torad );
  tgy[ 0 ][ 0 ] = cy;
  tgy[ 2 ][ 0 ] = sy;
  tgy[ 0 ][ 2 ] = -sy;
  tgy[ 2 ][ 2 ] = cy;

  fpnum cz = cosf( ang.z * torad );
  fpnum sz = sinf( ang.z * torad );
  tgz[ 0 ][ 0 ] = cz;
  tgz[ 1 ][ 0 ] = sz;
  tgz[ 0 ][ 1 ] = -sz;
  tgz[ 1 ][ 1 ] = cz;

  multiply( tgz );
  multiply( tgx );
  multiply( tgy );
}
inline void Matrix::unrotate( const Angle& ang ){
  static fpnum tf[ 16 ]    = { 1, 0, 0, 0, 
			       0, 1, 0, 0,
			       0, 0, 1, 0, 
			       0, 0, 0, 1 };
  static Matrix tgx( tf );
  static Matrix tgy( tf );
  static Matrix tgz( tf );
  fpnum cx = cosf( -ang.x * torad );
  fpnum sx = sinf( -ang.x * torad );
  tgx[ 1 ][ 1 ] = cx;
  tgx[ 2 ][ 1 ] = -sx;
  tgx[ 1 ][ 2 ] = sx;
  tgx[ 2 ][ 2 ] = cx;

  fpnum cy = cosf( -ang.y * torad );
  fpnum sy = sinf( -ang.y * torad );
  tgy[ 0 ][ 0 ] = cy;
  tgy[ 2 ][ 0 ] = sy;
  tgy[ 0 ][ 2 ] = -sy;
  tgy[ 2 ][ 2 ] = cy;

  fpnum cz = cosf( -ang.z * torad );
  fpnum sz = sinf( -ang.z * torad );
  tgz[ 0 ][ 0 ] = cz;
  tgz[ 1 ][ 0 ] = sz;
  tgz[ 0 ][ 1 ] = -sz;
  tgz[ 1 ][ 1 ] = cz;

  multiply( tgy );
  multiply( tgx );
  multiply( tgz );
}


inline const fpnum* Matrix::operator[]( size_t ind ) const{
  return grid[ ind ];
}
inline fpnum* Matrix::operator[]( size_t ind ){
  return grid[ ind ];
}
inline std::ostream& operator<<( std::ostream& o, const Matrix& m ){
  for( size_t j = 0; j < 4; j++ ){
    o << m[ 0 ][ j ] << 
      " " << m[ 1 ][ j ] <<
      " " << m[ 2 ][ j ] << 
      " " << m[ 3 ][ j ] << endl;
  }
  return o;
}

