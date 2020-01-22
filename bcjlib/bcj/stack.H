////////////////////////////////////////////////
// Stack template header
// (c) Jon DuBois 2004
// 03/3/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef STACK_H
#define STACK_H
#include "bcj/algo.H"



/////////////////////////////////////////////
// Stack
/////////////////////////////////////////////
// template< class T >
// class Stack{
// public:
//   struct Iterator{
//     Iterator( const Iterator& eq );
//     Iterator operator++( int );
//     Iterator operator++( void );
//     Iterator& operator+=( int a );
//     Iterator operator--( int );
//     Iterator operator--( void );
//     Iterator& operator+=( int a );
  
//     Iterator& operator=( const Iterator& eq );
//     bool operator==( const Iterator& eq ) const;
//     bool operator!=( const Iterator& eq ) const;
//     T& operator*( void ) const;
//     T* operator->( void ) const;

//     void swap( const Iterator& e );
      
//     friend class Stack;
//   private:
//     Iterator( T* d );
//     T** pointer;
//   };

//   Stack( void );
//   Stack( const Stack& cp );
//   ~Stack( void );

//   Stack& operator=( const Stack& cp );
//   bool operator==( const Stack& eq ) const;
//   bool operator!=( const Stack& eq ) const;
//   bool operator!( void ) const;  
//   const T& operator[]( size_t ind ) const;
//   T& operator[]( size_t ind );
//   size_t size( void ) const;
//   Iterator start( void ) const;
//   Iterator end( void ) const;


//   void push( const T& e );
//   void push( void );
//   const T& peek( void ) const;
//   T& peek( void );
//   void pop( void );
//   void empty( void );

//   friend std::ostream& operator<<( std::ostream& o, const Stack& s );
// };
template< class T >
class Stack{

  // initial size
  static const size_t init = 1;

  T** data;
  size_t sz, max;

public:

  struct Iterator{
    Iterator( const Iterator& eq ) : pointer( eq.pointer ){}
    Iterator operator++( void ){ return Iterator( ++pointer ); }
    Iterator operator++( int ){ return Iterator( pointer++ ); }
    Iterator& operator+=( int a ){ pointer += a; return *this; } 
    Iterator operator--( void ){ return Iterator( --pointer ); }
    Iterator operator--( int ){ return Iterator( pointer-- ); }
    Iterator& operator-=( int a ){ pointer -= a; return *this; } 
  
    Iterator& operator=( const Iterator& eq ){ pointer = eq.pointer; }
    bool operator==( const Iterator& eq ) const{ return pointer == eq.pointer; }
    bool operator!=( const Iterator& eq ) const{ return pointer != eq.pointer; }
    T& operator*( void ) const{ return **pointer; }
    T* operator->( void ) const{ return *pointer; }

    void swap( const Iterator& e ){ T* t( *( e.pointer ) ); *( e.pointer ) = *pointer; *pointer = t; }
      
    friend class Stack;
  private:
    Iterator( T** d ) : pointer( d ){}
    T** pointer;
  };

  Stack( void ) : data( new T*[ init ] ), sz( 0 ), max( init ) {}
  Stack( const Stack& cp ) : data( new T*[ cp.size() + 1 ] ), sz( cp.size() ), max( cp.size + 1 ){
    size_t i = sz;
    while( i-- ){
      data[ i ] = new T( *cp.data[ i ] );
    }
  }
  ~Stack( void ){
    size_t i = sz;
    while( i-- )
      delete data[ i ];
    delete data;
  }

    
  Stack& operator=( const Stack& cp ){
    size_t i = sz;
    while( i-- )
      delete data[ i ];
    delete data;
    data = new T*[ cp.sz + 1 ];
    max = cp.sz + 1;
    sz = cp.sz;
    i = sz;
    while( i-- ){
      data[ i ] = new T( *cp.data[ i ] );
    }
    return *this;
  }


  bool operator==( const Stack& eq ) const{
    if( eq.sz == sz ){
      for( size_t i = 0; i < sz; i++ )
	if( *data[ i ] != *eq.data[ i ] )
	  return false;
      return true;
    } else
      return false;
  }
  bool operator!=( const Stack& eq ) const{
    if( eq.sz == sz ){
      for( size_t i = 0; i < sz; i++ )
	if( *data[ i ] != *eq.data[ i ] )
	  return true;
      return false;
    } else
      return true;
  }
  bool operator!( void ) const{
    return !bool( sz );
  }

  
  const T& operator[]( size_t ind ) const{
    return *data[ ind ];
  }
  T& operator[]( size_t ind ){
    return *data[ ind ];
  }


  
  size_t size( void ) const{
    return sz;
  }
  Iterator start( void ) const{ return Iterator( &data[ 0 ] ); }
  Iterator end( void ) const{ return Iterator( &data[ sz - 1 ] ); }


  void push( const T& e ){
    if( sz >= max ){
      max *= 2;
      T** n = new T*[ max ];
      for( size_t i = 0; i < sz; i++ )
	n[ i ] = data[ i ];
      delete data;
      data = n;
    }
    data[ sz++ ] = new T( e );
  }
  void push( void ){
    if( sz >= max ){
      max *= 2;
      T** n = new T*[ max ];
      for( size_t i = 0; i < sz; i++ )
	n[ i ] = data[ i ];
      delete data;
      data = n;
    }
    data[ sz++ ] = new T;
  }
  const T& peek( void ) const{
    assess( sz, "Attempt to peek an empty Stack" );
    return *data[ sz - 1 ];
  }
  T& peek( void ){
    assess( sz, "Attempt to peek an empty Stack" );
    return *data[ sz - 1 ];
  }
  void pop( void ){
    assess( sz, "Attempt to pop an empty Stack" );
    delete data[ --sz ];
  }
  void empty( void ){
    *this = Stack();
  }

  friend std::ostream& operator<<( std::ostream& o, const Stack& s ){
    o << "stack<" << endl;
    Iterator ie( s.end() );
    Iterator ib( s.start() );
    do{
      cout << "  " << *ib << endl;
    }while( ib++ != ie );
    cout << ">" << endl;
    return o;
  }
};



#endif //STACK_H
