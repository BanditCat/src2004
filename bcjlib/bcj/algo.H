////////////////////////////////////////////////
// Algorithim template header
// (c) Jon DuBois 2004
// 03/3/2004
// This file is licensed via the GPL.
// See http://bcj.hopto.org/gpl.shtml or 
// license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef ALGO_H
#define ALGO_H



template< class T > 
void Sort( T b, T e ){
  if( b == e )
    return;

  T pb( b++ );
  T pe = e;
  do{
    while( ( *e >= *pb ) && ( b != e ) )
      e--;
    while( ( *b < *pb ) && ( b != e ) )
      b++;
    if( b != e )
      b.swap( e );
  } while( b != e );
  if( *pb > *b ){
    pb.swap( b );
    Sort( pb, --b );
    if( pe != e )
      Sort( ++e, pe );
  } else
    Sort( ++pb, pe );
}
template< class T >
inline void Sort( T& arg ){ 
  Sort( arg.start(), arg.end() );
}

 

/////////////////////////////////////////////////////////
// Pair.
/////////////////////////////////////////////////////////
//   template< class Key, Value >
//   class Pair{

//   Key key;
//   Value value;

//   Pair( const Key&, const Value& );
//   Pair( const Pair& );
//   ~Pair( void );

//   Pair& operator=( const Pair& );
  
//   bool operator==( const Pair& ) const;
//   bool operator!=( const Pair& ) const;

//   bool operator>( const Pair& ) const;
//   bool operator>=( const Pair& ) const;
//   bool operator<( const Pair& ) const;
//   bool operator<=( const Pair& ) const;

//   friend std::ostream& operator<<( std::ostream& o, const Pair& );
// }; 
template< class Key, class Value >
struct Pair{

  Key key;
  Value value;

  Pair( const Key& k, const Value& v ) : key( k ), value( v ) {}
  Pair( const Pair& cp ) : key( cp.key ), value( cp.value ) {}
  ~Pair( void ) {}

  Pair& operator=( const Pair& cp ){
    key = cp.key;
    value = cp.value;
    return *this;
  }
  
  bool operator==( const Pair& eq ) const{
    return key == eq.key;
  }
  bool operator!=( const Pair& eq ) const{
    return key != eq.key;
  }
  
  bool operator>( const Pair& eq ) const{
    return key > eq.key;
  }
  bool operator>=( const Pair& eq ) const{
    return key >= eq.key;
  }
  bool operator<( const Pair& eq ) const{
    return key < eq.key;
  }
  bool operator<=( const Pair& eq ) const{
    return key <= eq.key;
  }
  
  friend std::ostream& operator<<( std::ostream& o, const Pair& p ){
    o << "pair<" << p.key << ":" << p.value << ">";
    return o;
  }
};



#endif //ALGO_H

