////////////////////////////////////////////////
// Visualization engine header.
// (c) Jon DuBois 2004
// 10/24/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include <windows.h>


///////////////////////////////////////////////
// Handlers
///////////////////////////////////////////////
struct MouseEvent{
  static const byte leftbutton = 1;
  static const byte rightbutton = 2;
  static const byte middlebutton = 3;
  enum Type { pressed, released, doubleclicked } type; 
  byte button;
};
struct KeyEvent{
  bool pressed;
  byte symbol;
};
class Handler{
public:
  virtual ~Handler( void ){}
  virtual void mousehandler( const MouseEvent& ) = 0;
  virtual void keyhandler( const KeyEvent& ) = 0;
  virtual void tick( fpnum tcks ) = 0;
};




//////////////////////////////////////////////////
// Engine
//////////////////////////////////////////////////
class Engine{
public:
  Engine( dword icon );
  ~Engine( void );

  // This initializes the engine.
  bool init( word width, word height, byte bits, byte samples, bool anisotropic, const char* name, bool fullscreen );
  
  // The main function, returns false if it wants to quit.
  bool tick( void );
  void sethandler( Handler& );

  // Mouse & keyboard
  bool button( byte index ) const;
  bool key( byte index ) const;
  fpnum mousedx( void ) const;
  fpnum mousedy( void ) const;
  
  // Info
  fpnum fps( void ) const;
  dword fpsframes( void ) const;
  void fpsframes( dword );
  // The initial value for fpsframes.
  static const dword initfpsframes = 20;
  bool focused( void ) const;
  fpnum aspect( void ) const;
  dword width( void ) const;
  dword height( void ) const;

  // The screen
  Display screen;
private:
  dword pfpsframes;
  static bool& exists( void ){ static bool pexists = false; return pexists; }

  // Windows functions
  void killwindow( void );
  bool createwindow( word width, word hieght, byte bits, byte samples, const char* name, bool fullscreen );
  bool initmultisample( byte samples, byte bits );
};



#endif //ENGINE_H
