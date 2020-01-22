////////////////////////////////////////////////
// Visualization v2 geometry header.
// (c) Jon DuBois 2004
// 10/22/2004
// This file is licensed via the GPL.
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
  Vector( void ){}
  // Creates a unit vector pointing in the same direction
  Vector( const Angle& a );
  Vector( fpnum xp, fpnum yp, fpnum zp ) : x( xp ), y( yp ), z( zp ){}
  ~Vector( void ){}

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

  fpnum x;
  fpnum y;
  fpnum z;
};


struct Matrix{
  Matrix( void );
  Matrix( fpnum* fp );

  void identity( void );
  void translate( const Vector& );
  void rotate( const Angle& );
  void unrotate( const Angle& );
  void multiply( const Matrix& );

  const fpnum* operator[]( size_t ind ) const;
  fpnum* operator[]( size_t ind );
  friend std::ostream& operator<<( std::ostream&, const Matrix& );

private:
  fpnum grid[ 4 ][ 4 ];
};


#include "geometry.cpp"
#endif //GEOMETRY_H
