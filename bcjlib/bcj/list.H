// template< class T >
// class List{
// public:
//   struct Iterator{
//     Iterator( const Iterator& eq );
//     Iterator operator++( void );
//     Iterator operator++( int );
//     Iterator& operator+=( int a );
//     Iterator operator--( void );
//     Iterator operator--( int );
//     Iterator& operator-=( int a );
  
//     Iterator& operator=( const Iterator& eq );
//     bool operator==( const Iterator& eq ) const;
//     bool operator!=( const Iterator& eq ) const;
//     T& operator*( void ) const;
//     T* operator->( void ) const;

//     void swap( const Iterator& e );
      
//     friend class List;
//   };

//   List( void );
//   ~List( void );

//   void push( const T& d );
//   void push( void );
//   void pushback( const T& d );
//   void pushback( void );
//   void pop( void );
//   void popback( void );
//   T& peek( void ) const;
//   T& peekback( void ) const;
//   void insert( Iterator& e, const T& d );
//   void insert( Iterator& e );
//   void remove( Iterator& e );
//   size_t size( void ) const;
//   void empty( void )

//   Iterator start( void ) const;
//   Iterator end( void ) const;
// };
template< class T >
class List{
  struct Node{
    Node( Node* p, Node* n, T* d ) : previous( p ), next( n ), data( d ){}
    Node* previous;
    Node* next;
    T* data;
  }; 
public:
  struct Iterator{
    Iterator( const Iterator& eq ) : pointer( eq.pointer ){}
    Iterator operator++( void ){ pointer = pointer->next; return *this; }
    Iterator operator++( int ){ Iterator i( *this ); ++*this; return i; }
    Iterator& operator+=( int a ){ 
      if( a >= 0 )
	while( a-- )
	  pointer = pointer->next;
      else
	while( a++ )
	  pointer = pointer->previous;
      return *this;  
    } 
    Iterator operator--( void ){ pointer = pointer->previous; return *this; }
    Iterator operator--( int ){ Iterator i( *this ); --*this; return i; }
    Iterator& operator-=( int a ){ *this += -a; return *this; } 
  
    Iterator& operator=( const Iterator& eq ){ pointer = eq.pointer; }
    bool operator==( const Iterator& eq ) const{ return pointer == eq.pointer; }
    bool operator!=( const Iterator& eq ) const{ return pointer != eq.pointer; }
    T& operator*( void ) const{ return *pointer->data; }
    T* operator->( void ) const{ return pointer->data; }

    void swap( const Iterator& e ){ 
      T* t = e.pointer->data; 
      e.pointer->data = pointer->data; 
      pointer->data = t;
    }
      
    friend class List;
  private:
    Iterator( Node* d ) : pointer( d ){}
    Node* pointer;
  };

  List( void ) : startp( new Node( NULL, NULL, NULL ) ), 
		 endp( new Node( startp, NULL, NULL ) ), psize( 0 ){ 
    startp->next = endp; 
  }
  ~List( void ){ 
    while( startp != NULL ){
      if( startp->data != NULL )
        delete startp->data;
      Node* t( startp );
      startp = startp->next;
      delete t;
    }
  }

  void push( const T& d ){
    psize++;
    Node* n = new Node( endp->previous, endp, new T( d ) );
    n->previous->next = n;
    n->next->previous = n;
  }
  void push( void ){
    psize++;
    Node* n = new Node( endp->previous, endp, new T() );
    n->previous->next = n;
    n->next->previous = n;
  }
  void pushback( const T& d ){
    psize++;
    Node* n = new Node( startp, startp->next, new T( d ) ); 
    n->previous->next = n;
    n->next->previous = n;
  }
  void pushback( void ){
    psize++;
    Node* n = new Node( startp, startp->next, new T() );
    n->previous->next = n;
    n->next->previous = n;
  } 
  void pop( void ){
    psize--;
    Node* t( startp->next );
    startp->next = startp->next->next;
    delete t->data;
    delete t;
  }
  void popback( void ){
    psize--;
    Node* t( endp->previous );
    endp->previous = startp->previous->previous;
    delete t->data;
    delete t;
  }
  T& peek( void ){ return *endp->previous->data; }
  T& peekback( void ){ return *startp->next->data; }
  void insert( Iterator& e, const T& d ){
    psize++;
    Node* n = new Node( e.pointer->previous, e.pointer, new T( d ) );
    n->previous->next = n;
    n->next->previous = n;
    --e;
  }
  void insert( Iterator& e ){
    psize++;
    Node* n = new Node( e.pointer->previous, e.pointer, new T() );
    n->previous->next = n;
    n->next->previous = n;
    --e;
  }
  void remove( Iterator& e ){
    psize--;
    e.pointer->next->previous = e.pointer->previous;
    Node* t( e.pointer->previous->next = e.pointer->next );
    delete e.pointer->data;
    delete e.pointer;
    e.pointer = t;
  }
  size_t size( void ) const{ return psize; }
  void empty( void ){
    while( startp->next != endp ){
      Node* t( startp );
      startp = startp->next;
      if( startp->data != NULL ){
	delete startp->data;
	startp->data = NULL;
      }
      delete t;
    }
    psize = 0;
  }

  Iterator start( void ) const{ return Iterator( startp->next ); }
  Iterator end( void ) const{ return Iterator( endp->previous ); }

  friend std::ostream& operator<<( std::ostream& o, const List& l ){
    o << "list<" << endl;
    Iterator ie( l.end() );
    Iterator ib( l.start() );
    do{
      cout << "  " << *ib << endl;
    }while( ib++ != ie );
    cout << ">" << endl;
    return o;
  }
private: 
  Node *startp, *endp;
  size_t psize;
};
  
