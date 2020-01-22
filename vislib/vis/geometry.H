////////////////////////////////////////////////
// Visualization v2 geometry header.
// (c) Jon DuBois 2004
// 10/22/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef GEOMETRY_H
#define GEOMETRY_H
#include <iostream>
#include "vis/vis.H"



/////////////////////////////////////////////////
// Geometry etc
/////////////////////////////////////////////////
struct Matrix;
struct Angle{
  Angle( void ){}
  Angle( fpnum xr, fpnum yr, fpnum zr ) : x( xr ), y( yr ), z( zr ){}
  ~Angle( void ){}

  fpnum x;
  fpnum y;
  fpnum z;
};


struct Vector{
  Vector( void );
  // Creates a unit vector pointing in the same direction
  Vector( const Angle& a );
  Vector( const Vector& );
  Vector( fpnum xp, fpnum yp, fpnum zp );
  Vector& operator=( const Vector& );
  ~Vector( void );

  fpnum length( void ) const;
  fpnum dot( const Vector& ) const;
  fpnum component( const Vector& ) const;

  void project( const Vector& );
  void cross( const Vector& );
  void normal( void );
  void rotate( const Angle& ang );
  void unrotate( const Angle& ang );
  void transform( const Matrix& mt );

  Vector& operator*=( fpnum scl );
  Vector& operator/=( fpnum scl );
  Vector& operator+=( const Vector& );
  Vector& operator-=( const Vector& );

  friend std::ostream& operator<<( std::ostream&, const Vector& );

  fpnum x;
  fpnum y;
  fpnum z;
};


struct Matrix{
  Matrix( void );
  Matrix( fpnum* fp );
  Matrix( const Matrix& );
  Matrix& operator=( const Matrix& );
  ~Matrix( void );

  void identity( void );
  void translate( const Vector& );
  // This function removes all translation from the matrix.
  void originate( void );
  void scale( const Vector& );
  void rotate( const Angle& );
  // This rotates around a vector, it must be a unit vector.
  void rotate( const Vector&, fpnum );
  void unrotate( const Angle& );
  void invert( void );
  void multiply( const Matrix& );
  void imultiply( const Matrix& );

  const fpnum* operator[]( size_t ind ) const;
  friend std::ostream& operator<<( std::ostream&, const Matrix& );

private:
  void pmultiply( fpnum*, const fpnum* );
  void pimultiply( fpnum*, const fpnum* );
  fpnum* grid;
  fpnum* inverse;
};







////////////////////////////////////////////////
// Here is the implementation.
////////////////////////////////////////////////
inline Vector::Vector( void ){}
inline Vector::Vector( const Vector& eq ){
  x = eq.x;
  y = eq.y;
  z = eq.z;
}
inline Vector::Vector( const Angle& ang ){
  x = sinf( ang.y * torad ) * cosf( ang.x * torad );
  z = cosf( ang.y * torad ) * cosf( ang.x * torad );
  y = -sinf( ang.x * torad );
}
inline Vector::Vector( fpnum xp, fpnum yp, fpnum zp ) : x( xp ), y( yp ), z( zp ){
}
inline Vector& Vector::operator=( const Vector& eq ){
  x = eq.x;
  y = eq.y;
  z = eq.z;
  return *this;
}
inline Vector::~Vector( void ){}




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
  Vector u( v );
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



inline std::ostream& operator<<( std::ostream& o, const Vector& v ){
  o << "Vector< " << v.x << " " << v.y << " " << v.z << " >" << std::endl;
  return o;
}



inline Matrix::Matrix( void ) : grid( new fpnum[ 16 ] ), inverse( new fpnum[ 16 ] ){}
inline Matrix::Matrix( const Matrix& eq ) : grid( new fpnum[ 16 ] ), inverse( new fpnum[ 16 ] ){
  size_t i = 16;
  while( i-- ){
    grid[ i ] = eq.grid[ i ];
    inverse[ i ] = eq.inverse[ i ];
  }
}
inline Matrix& Matrix::operator=( const Matrix& eq ){
  delete[] grid;
  delete[] inverse;
  grid = new fpnum[ 16 ];
  inverse = new fpnum[ 16 ];
  size_t i = 16;
  while( i-- ){
    grid[ i ] = eq.grid[ i ];
    inverse[ i ] = eq.inverse[ i ];
  }
  return *this;
}
inline Matrix::~Matrix( void ){
  delete[] grid;
  delete[] inverse;
}




inline void Matrix::identity( void ){
  static fpnum tf[ 16 ] = { 1, 0, 0, 0, 
			    0, 1, 0, 0,
			    0, 0, 1, 0,
			    0, 0, 0, 1 };
  size_t i = 16;
  while( i-- ){
    grid[ i ] = tf[ i ];
    inverse[ i ] = tf[ i ];
  }
}
inline void Matrix::translate( const Vector& v ){
  static fpnum tf[ 16 ] = { 1, 0, 0, 0, 
			    0, 1, 0, 0,
			    0, 0, 1, 0,
			    0, 0, 0, 1 };
  tf[ 3 * 4 + 0 ] = v.x;
  tf[ 3 * 4 + 1 ] = v.y;
  tf[ 3 * 4 + 2 ] = v.z;
  pmultiply( grid, tf );

  tf[ 3 * 4 + 0 ] = -v.x;
  tf[ 3 * 4 + 1 ] = -v.y;
  tf[ 3 * 4 + 2 ] = -v.z;
  pimultiply( inverse, tf );
}
inline void Matrix::originate( void ){
  grid[ 3 * 4 + 0 ] = 0;
  grid[ 3 * 4 + 1 ] = 0;
  grid[ 3 * 4 + 2 ] = 0;
  inverse[ 3 * 4 + 0 ] = 0;
  inverse[ 3 * 4 + 1 ] = 0;
  inverse[ 3 * 4 + 2 ] = 0;
}
inline void Matrix::scale( const Vector& v ){
  static fpnum tf[ 16 ] = { 1, 0, 0, 0, 
			    0, 1, 0, 0,
			    0, 0, 1, 0,
			    0, 0, 0, 1 };
  assess( v.x && v.y && v.z, "Matrix::scale called with a vector with a zero component" );
  tf[ 0 * 4 + 0 ] = v.x;
  tf[ 1 * 4 + 1 ] = v.y;
  tf[ 2 * 4 + 2 ] = v.z;
  pmultiply( grid, tf );

  tf[ 0 * 4 + 0 ] = 1 / v.x;
  tf[ 1 * 4 + 1 ] = 1 / v.y;
  tf[ 2 * 4 + 2 ] = 1 / v.z;
  pimultiply( inverse, tf );
}
inline void Matrix::rotate( const Angle& ang ){
  static fpnum tgx[ 16 ]  = { 1, 0, 0, 0, 
			      0, 1, 0, 0,
			      0, 0, 1, 0, 
			      0, 0, 0, 1 };
  static fpnum tgy[ 16 ]  = { 1, 0, 0, 0, 
			      0, 1, 0, 0,
			      0, 0, 1, 0, 
			      0, 0, 0, 1 };
  static fpnum tgz[ 16 ]  = { 1, 0, 0, 0, 
			      0, 1, 0, 0,
			      0, 0, 1, 0, 
			      0, 0, 0, 1 };
  fpnum cx = cosf( ang.x * torad );
  fpnum sx = sinf( ang.x * torad );
  tgx[ 1 * 4 + 1 ] = cx;
  tgx[ 2 * 4 + 1 ] = -sx;
  tgx[ 1 * 4 + 2 ] = sx;
  tgx[ 2 * 4 + 2 ] = cx;

  fpnum cy = cosf( ang.y * torad );
  fpnum sy = sinf( ang.y * torad );
  tgy[ 0 * 4 + 0 ] = cy;
  tgy[ 2 * 4 + 0 ] = sy;
  tgy[ 0 * 4 + 2 ] = -sy;
  tgy[ 2 * 4 + 2 ] = cy;

  fpnum cz = cosf( ang.z * torad );
  fpnum sz = sinf( ang.z * torad );
  tgz[ 0 * 4 + 0 ] = cz;
  tgz[ 1 * 4 + 0 ] = sz;
  tgz[ 0 * 4 + 1 ] = -sz;
  tgz[ 1 * 4 + 1 ] = cz;

  pmultiply( grid, tgz );
  pmultiply( grid, tgx );
  pmultiply( grid, tgy );

  tgx[ 2 * 4 + 1 ] = sx;
  tgx[ 1 * 4 + 2 ] = -sx;
  tgy[ 2 * 4 + 0 ] = -sy;
  tgy[ 0 * 4 + 2 ] = sy;
  tgz[ 1 * 4 + 0 ] = -sz;
  tgz[ 0 * 4 + 1 ] = sz;

  pimultiply( inverse, tgz );
  pimultiply( inverse, tgx );
  pimultiply( inverse, tgy );
}
inline void Matrix::rotate( const Vector& v, fpnum a ){
  fpnum tarr[ 16 ] = { 1, 0, 0, 0,
		       0, 1, 0, 0,
		       0, 0, 1, 0,
		       0, 0, 0, 1 };
  fpnum c = cosf( a * torad );
  fpnum s = sinf( a * torad );
  fpnum t = 1 - c;
  
  const fpnum& x = v.x;
  const fpnum& y = v.y;
  const fpnum& z = v.z;

  tarr[ 0 * 4 + 0 ] = t * x * x + c;
  tarr[ 0 * 4 + 1 ] = t * x * y + s * z;
  tarr[ 0 * 4 + 2 ] = t * x * z - s * y;

  tarr[ 1 * 4 + 0 ] = t * x * y - s * z;
  tarr[ 1 * 4 + 1 ] = t * y * y + c;
  tarr[ 1 * 4 + 2 ] = t * y * z + s * x;

  tarr[ 2 * 4 + 0 ] = t * x * z + s * y;
  tarr[ 2 * 4 + 1 ] = t * y * z - s * x;
  tarr[ 2 * 4 + 2 ] = t * z * z + c;

  pmultiply( grid, tarr );

  tarr[ 0 * 4 + 1 ] -= 2 * s * z;
  tarr[ 0 * 4 + 2 ] += 2 * s * y;
  tarr[ 1 * 4 + 0 ] += 2 * s * z;
  tarr[ 1 * 4 + 2 ] -= 2 * s * x;
  tarr[ 2 * 4 + 0 ] -= 2 * s * y;
  tarr[ 2 * 4 + 1 ] += 2 * s * x;

  pimultiply( inverse, tarr );
}
inline void Matrix::unrotate( const Angle& ang ){
  static fpnum tgx[ 16 ]  = { 1, 0, 0, 0, 
			      0, 1, 0, 0,
			      0, 0, 1, 0, 
			      0, 0, 0, 1 };
  static fpnum tgy[ 16 ]  = { 1, 0, 0, 0, 
			      0, 1, 0, 0,
			      0, 0, 1, 0, 
			      0, 0, 0, 1 };
  static fpnum tgz[ 16 ]  = { 1, 0, 0, 0, 
			      0, 1, 0, 0,
			      0, 0, 1, 0, 
			      0, 0, 0, 1 };
  fpnum cx = cosf( ang.x * torad );
  fpnum sx = sinf( -ang.x * torad );
  tgx[ 1 * 4 + 1 ] = cx;
  tgx[ 2 * 4 + 1 ] = -sx;
  tgx[ 1 * 4 + 2 ] = sx;
  tgx[ 2 * 4 + 2 ] = cx;

  fpnum cy = cosf( ang.y * torad );
  fpnum sy = sinf( -ang.y * torad );
  tgy[ 0 * 4 + 0 ] = cy;
  tgy[ 2 * 4 + 0 ] = sy;
  tgy[ 0 * 4 + 2 ] = -sy;
  tgy[ 2 * 4 + 2 ] = cy;

  fpnum cz = cosf( ang.z * torad );
  fpnum sz = sinf( -ang.z * torad );
  tgz[ 0 * 4 + 0 ] = cz;
  tgz[ 1 * 4 + 0 ] = sz;
  tgz[ 0 * 4 + 1 ] = -sz;
  tgz[ 1 * 4 + 1 ] = cz;

  pmultiply( grid, tgy );
  pmultiply( grid, tgx );
  pmultiply( grid, tgz );

  tgx[ 2 * 4 + 1 ] = sx;
  tgx[ 1 * 4 + 2 ] = -sx;
  tgy[ 2 * 4 + 0 ] = -sy;
  tgy[ 0 * 4 + 2 ] = sy;
  tgz[ 1 * 4 + 0 ] = -sz;
  tgz[ 0 * 4 + 1 ] = sz;

  pimultiply( inverse, tgy );
  pimultiply( inverse, tgx );
  pimultiply( inverse, tgz );
}
inline void Matrix::invert( void ){
  fpnum* t = grid;
  grid = inverse;
  inverse = t;
}
inline void Matrix::multiply( const Matrix& mat ){
  pmultiply( grid, mat.grid );
  pimultiply( inverse, mat.inverse );
}
inline void Matrix::imultiply( const Matrix& mat ){
  pimultiply( grid, mat.grid );
  pmultiply( inverse, mat.inverse );
}



inline const fpnum* Matrix::operator[]( size_t ind ) const{ 
  assess( ( ind >= 0 ) && ( ind <= 3 ), "Index out of bounds for Matrix::operator[]" );
  return grid + ind * 4; 
}



inline std::ostream& operator<<( std::ostream& o, const Matrix& m ){
  o << "Matrix<" << std::endl;
  for( size_t j = 0; j < 4; j++ ){
    o << m[ 0 ][ j ] << 
      " " << m[ 1 ][ j ] <<
      " " << m[ 2 ][ j ] << 
      " " << m[ 3 ][ j ] << std::endl;
  }
  o <<  ">" << std::endl;
  return o;
}



inline void Matrix::pmultiply( fpnum* tm, const fpnum* m ){
  static fpnum tg[ 16 ];
  for( size_t i = 0; i < 4; i++ ){
    for( size_t j = 0; j < 4; j++ ){
      tg[ i * 4 + j ] = 
	tm[ i * 4 + 0 ] * m[ 0 * 4 + j ] +
	tm[ i * 4 + 1 ] * m[ 1 * 4 + j ] +
	tm[ i * 4 + 2 ] * m[ 2 * 4 + j ] +
	tm[ i * 4 + 3 ] * m[ 3 * 4 + j ];
    }
  }
  size_t i = 16;
  while( i-- )
    tm[ i ] = tg[ i ];
  
}
inline void Matrix::pimultiply( fpnum* tm, const fpnum* m ){
  static fpnum tg[ 16 ];
  for( size_t i = 0; i < 4; i++ ){
    for( size_t j = 0; j < 4; j++ ){
      tg[ i * 4 + j ] = 
	m[ i * 4 + 0 ] * tm[ 0 * 4 + j ] +
	m[ i * 4 + 1 ] * tm[ 1 * 4 + j ] +
	m[ i * 4 + 2 ] * tm[ 2 * 4 + j ] +
	m[ i * 4 + 3 ] * tm[ 3 * 4 + j ];
    }
  }
  size_t i = 16;
  while( i-- )
    tm[ i ] = tg[ i ];
}



#endif //GEOMETRY_H
