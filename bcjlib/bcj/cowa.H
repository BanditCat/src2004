////////////////////////////////////////////////
// Copy on write array template header
// (c) Jon DuBois 2004
// 03/3/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
//
// This is a generalized implementation of the
// copy-on-write style of array managment.
// A Cowa is an agragate that may treated 
// atomicly; specifically, it does not behave
// as a pointer, i.e. A == B does not imply that
// a change to A also changes B.  The only non
// intuitive interface element is the 
// constructors; A Cowa may not be directly 
// copied from a T*, instead, you either create
// am empty Cowa of fixed size by calling
// the constructor with a size_t, then fill it
// manually, or you call the constructor like 
// this: Cowa( const T* p, szcp, sz ); The later
// form creates a Cowa of size sz with its 
// first szcp elements copied over from p.
// Also, the only way to change the elements
// of a Cowa is through a call to 
// set( ind, dat ), a normal index lookup
// will not return a lvalue.  The set method 
// also has an overloaded three argument
// version; the first is a starting index,
// the second is the number of elements to copy
// and the third is an array of T's to copy in.
////////////////////////////////////////////////
#ifndef COWA_H
#define COWA_H


 
/////////////////////////////////////////////
// Cowa
/////////////////////////////////////////////
// template< class T >
// class Cowa{

//   //No default contructor
//   Cowa( void );
  
//   struct Pntr{
    
//     size_t size;
//     T* data;
//     size_t refs;

//     Pntr( size_t sz );
//     Pntr( const Pntr& p );
//     Pntr( const T*, size_t, size_t );
//     Pntr( const T*, size_t );
//     ~Pntr( void );
//   }* pntr;
  
//   void decref( void );
//   void detach( void );

// public:

//   Cowa( const Cowa& cp );
//   Cowa( size_t sz );
//   Cowa( const T* cp, size_t cpsz, size_t sz );
//   // This takes over an array, and is therefore dangerous
//   Cowa( const T* cp, size_t sz );

//   Cowa& operator=( const Cowa& eq );
  
//   operator const T*( void ) const;
//   bool operator==( const Cowa& eq ) const;
//   bool operator!=( const Cowa& eq ) const;

//   void set( size_t ind, const T& dat );
//   void set( size_t sind, size_t len, const T* dat );
//   size_t size( void ) const;
// }; 

template< class T >
class Cowa{

  //No default contructor
  Cowa( void );
  
  struct Pntr{

    size_t size;
    T* data;
    size_t refs; 
   
    Pntr( size_t sz ) : size( sz ), data( new T[ sz ] ), refs( 0 ){}
    Pntr( const Pntr& p ) : size( p.size ), data( new T[ p.size ] ), refs( 0 ){
      for( size_t i = 0; i < size; i++ )
	data[ i ] = p.data[ i ]; 
    } 
    Pntr( const T* cp, size_t cpsz, size_t sz ) : size( sz ), data( new T[ sz ] ), refs( 0 ){
      assess( cpsz <= sz, "Buffer overflow in subarray constructor for Cowa" );
      for( size_t i = 0; i < cpsz; i++ )
	data[ i ] = cp[ i ];
    }
    Pntr( T* cp, size_t sz ) : size( sz ), data( cp ), refs( 0 ){}
    ~Pntr( void ) {
      delete[] data;
    }
    
  }* pntr;
  
  void decref( void ){
    if( !( pntr->refs-- ) )
      delete pntr;
  }
  void detach( void ){
    if( pntr->refs ){
      decref();
      pntr = new Pntr( *pntr );
    }
  }

public:

  Cowa( const Cowa& cp ) : pntr( cp.pntr ){
    cp.pntr->refs++;
  }
  Cowa( size_t sz ) : pntr( new Pntr( sz ) ) {}
  ~Cowa( void ){
    decref();
  }
  Cowa( const T* cp, size_t cpsz, size_t sz ) : 
    pntr( new Pntr( cp, cpsz, sz ) ){}
  Cowa( T* cp, size_t sz ) : pntr( new Pntr( cp, sz ) ){}
    

  Cowa& operator=( const Cowa& eq ){
    if( &eq != this ){
      decref();
      pntr = eq.pntr;
      pntr->refs++;
    }
    return *this;
  } 
  
  operator const T*( void ) const{
    return pntr->data;
  }

  bool operator==( const Cowa& eq ) const{
    if( pntr == eq.pntr )
      return true;
    for( size_t i = 0; i < pntr->size; i++ )
      if( pntr->data[ i ] != eq.pntr->data[ i ] )
	return false;
    return true;
  }
  bool operator!=( const Cowa& eq ) const{
    if( pntr == eq.pntr )
      return false;
    for( size_t i = 0; i < pntr->size; i++ )
      if( pntr->data[ i ] != eq.pntr->data[ i ] )
	return true;
    return false;
  }
 
  void set( size_t ind, const T& dat ){
    detach();
    pntr->data[ ind ] = dat;
  }
  void set( size_t sind, size_t len, const T* dat ){
    detach();
    for( size_t i = 0; i < len ; i++ )
      pntr->data[ i + sind ] = dat[ i ];
  }
  size_t size( void ) const{
    return pntr->size;
  }
}; 



#endif //COWA_H

