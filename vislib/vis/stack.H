//////////////////////////////////////////////////////
// vis v2 stack source file.
// (c) Jon DuBois 2004
// 11/02/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#ifndef STACK_H
#define STACK_H



/////////////////////////////////////////////
// Stack for objects
/////////////////////////////////////////////
// template< class T >
// class ObjStack{
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
      
//     friend class ObjStack;
//   };

//   ObjStack( void );
//   ObjStack( const ObjStack& cp );
//   ~ObjStack( void );

//   ObjStack& operator=( const ObjStack& cp );
//   bool operator==( const ObjStack& eq ) const;
//   bool operator!=( const ObjStack& eq ) const;
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

//   friend std::ostream& operator<<( std::ostream& o, const ObjStack& s );
// };
template< class T >
class ObjStack{

  // initial size
  static const size_t init = 1;

  T** data;
  size_t sz, max;

public:

  struct Iterator{
    Iterator( const Iterator& eq ) : pointer( eq.pointer ), data( eq.data ){}
    Iterator operator++( void ){ return Iterator( ++pointer, data ); }
    Iterator operator++( int ){ return Iterator( pointer++, data ); }
    Iterator& operator+=( int a ){ pointer += a; return *this; } 
    Iterator operator--( void ){ return Iterator( --pointer, data ); }
    Iterator operator--( int ){ return Iterator( pointer--, data ); }
    Iterator& operator-=( int a ){ pointer -= a; return *this; } 
  
    Iterator& operator=( const Iterator& eq ){ pointer = eq.pointer, data = eq.data; }
    bool operator==( const Iterator& eq ) const{ return ( pointer == eq.pointer ) && ( data == eq.data ); }
    bool operator!=( const Iterator& eq ) const{ return ( pointer != eq.pointer ) || ( data != eq.data ); }
    T& operator*( void ) const{ return *( ( *data )[ pointer ] ); }
    T* operator->( void ) const{ return ( *data )[ pointer ]; }

    void swap( const Iterator& e ){ 
      T* t( ( *data )[ e.pointer ] ); 
      ( *e.data )[ e.pointer ] = ( *data )[ pointer ]; 
      ( *data )[ pointer ] = t; 
    }
      
    friend class ObjStack;
  private:
    Iterator( size_t i, T** const* d ) : pointer( i ), data( d ){}
    size_t pointer;
    T** const* data;
  };

  ObjStack( void ) : data( new T*[ init ] ), sz( 0 ), max( init ) {}
  ObjStack( const ObjStack& cp ) : data( new T*[ cp.size() + 1 ] ), sz( cp.size() ), max( cp.size + 1 ){
    size_t i = sz;
    while( i-- ){
      data[ i ] = new T( *cp.data[ i ] );
    }
  }
  ~ObjStack( void ){
    size_t i = sz;
    while( i-- )
      delete data[ i ];
    delete data;
  }

    
  ObjStack& operator=( const ObjStack& cp ){
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


  bool operator==( const ObjStack& eq ) const{
    if( eq.sz == sz ){
      for( size_t i = 0; i < sz; i++ )
	if( *data[ i ] != *eq.data[ i ] )
	  return false;
      return true;
    } else
      return false;
  }
  bool operator!=( const ObjStack& eq ) const{
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
  Iterator start( void ) const{ return Iterator( 0, &data ); }
  Iterator end( void ) const{ return Iterator( sz - 1, &data ); }


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
    assess( sz, "Attempt to peek an empty ObjStack" );
    return *data[ sz - 1 ];
  }
  T& peek( void ){
    assess( sz, "Attempt to peek an empty ObjStack" );
    return *data[ sz - 1 ];
  }
  void pop( void ){
    assess( sz, "Attempt to pop an empty ObjStack" );
    delete data[ --sz ];
  }
  void empty( void ){
    *this = ObjStack();
  }

  friend std::ostream& operator<<( std::ostream& o, const ObjStack& s ){
    o << "ObjStack<" << endl;
    Iterator ie( s.end() );
    Iterator ib( s.start() );
    do{
      cout << "  " << *ib << endl;
    }while( ib++ != ie );
    cout << ">" << endl;
    return o;
  }
};



/////////////////////////////////////////////
// Stack for builtins
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

  T* data;
  size_t sz, max;

public:

  struct Iterator{
    Iterator( const Iterator& eq ) : pointer( eq.pointer ), data( eq.data ){}
    Iterator operator++( void ){ return Iterator( ++pointer, data ); }
    Iterator operator++( int ){ return Iterator( pointer++, data ); }
    Iterator& operator+=( int a ){ pointer += a; return *this; } 
    Iterator operator--( void ){ return Iterator( --pointer, data ); }
    Iterator operator--( int ){ return Iterator( pointer--, data ); }
    Iterator& operator-=( int a ){ pointer -= a; return *this; } 
  
    Iterator& operator=( const Iterator& eq ){ pointer = eq.pointer, data = eq.data; }
    bool operator==( const Iterator& eq ) const{ return ( pointer == eq.pointer ) && ( data == eq.data ); }
    bool operator!=( const Iterator& eq ) const{ return ( pointer != eq.pointer ) || ( data != eq.data ); }
    T operator*( void ) const{ return *( *data + pointer ); }
    T* operator->( void ) const{ return *data + pointer; }

    void swap( const Iterator& e ){ 
      T t( ( *data )[ e.pointer ] ); 
      ( *e.data )[ e.pointer ] = ( *data )[ pointer ]; 
      ( *data )[ pointer ] = t; 
    }
      
    friend class Stack;
  private:
    Iterator( size_t i, T* const* d ) : pointer( i ), data( d ){}
    size_t pointer;
    T* const* data;
  };

  Stack( void ) : data( new T[ init ] ), sz( 0 ), max( init ) {}
  Stack( const Stack& cp ) : data( new T[ cp.size() + 1 ] ), sz( cp.size() ), max( cp.size + 1 ){
    size_t i = sz;
    while( i-- ){
      data[ i ] = cp.data[ i ];
    }
  }
  ~Stack( void ){
    delete data;
  }

    
  Stack& operator=( const Stack& cp ){
    delete data;
    data = new T[ cp.sz + 1 ];
    max = cp.sz + 1;
    sz = cp.sz;
    size_t i = sz;
    while( i-- ){
      data[ i ] = cp.data[ i ];
    }
    return *this;
  }


  bool operator==( const Stack& eq ) const{
    if( eq.sz == sz ){
      for( size_t i = 0; i < sz; i++ )
	if( data[ i ] != eq.data[ i ] )
	  return false;
      return true;
    } else
      return false;
  }
  bool operator!=( const Stack& eq ) const{
    if( eq.sz == sz ){
      for( size_t i = 0; i < sz; i++ )
	if( data[ i ] != eq.data[ i ] )
	  return true;
      return false;
    } else
      return true;
  }
  bool operator!( void ) const{
    return !bool( sz );
  }

  
  const T& operator[]( size_t ind ) const{
    return data[ ind ];
  }
  T& operator[]( size_t ind ){
    return data[ ind ];
  }


  
  size_t size( void ) const{
    return sz;
  }
  Iterator start( void ) const{ return Iterator( 0, &data ); }
  Iterator end( void ) const{ return Iterator( sz - 1, &data ); }


  void push( const T& e ){
    if( sz >= max ){
      max *= 2;
      T* n = new T[ max ];
      for( size_t i = 0; i < sz; i++ )
	n[ i ] = data[ i ];
      delete data;
      data = n;
    }
    data[ sz++ ] = e;
  }
  void push( void ){
    if( sz >= max ){
      max *= 2;
      T* n = new T[ max ];
      for( size_t i = 0; i < sz; i++ )
	n[ i ] = data[ i ];
      delete data;
      data = n;
    }
    sz++;
  }
  T& peek( void ){
    assess( sz, "Attempt to peek an empty Stack" );
    return data[ sz - 1 ];
  }
  const T& peek( void ) const{
    assess( sz, "Attempt to peek an empty Stack" );
    return data[ sz - 1 ];
  }
  void pop( void ){
    assess( sz, "Attempt to pop an empty Stack" );
    --sz;
  }
  void empty( void ){
    *this = Stack();
  }

  friend std::ostream& operator<<( std::ostream& o, const Stack& s ){
    o << "Stack<" << endl;
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
