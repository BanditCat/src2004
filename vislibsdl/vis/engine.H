////////////////////////////////////////////////
// Visualization engine header.
// (c) Jon DuBois 2004
// 02/25/2004
// This file is licensed via the GPL.
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////
#ifndef ENGINE_H
#define ENGINE_H
#include <iostream>
#include "vis/vis.H"


//////////////////////////////////////////////////
// Engine
//////////////////////////////////////////////////
class Engine{
public:
  Engine( word, word, const char* );
  ~Engine( void );

  // The main function;
  void tick( void );
  void sethandler( Handler& );

  // Mouse
  fpnum mousedx( void ) const{ return mdx; }
  fpnum mousedy( void ) const{ return mdy; }
  byte buttons( void ) const{ return bs; }
  
  // Info
  fpnum fps( void ) const{ return afps; }

  Display screen;

private:
  // These are used to calculate the average fps over the last 20 frames.
  fpnum afps;
  dword atcks[ 20 ];
  byte apos;

  bool fcs;
  byte bs;
  float mdx, mdy;
  size_t bpp;
  static bool& exists( void ){ static bool pexists = false; return pexists; }
  Handler* hndlr;
  // The number of ticks last time tick was called.
  dword ltcks;
};
// bool Engine::exists = false;



#endif //ENGINE_H
