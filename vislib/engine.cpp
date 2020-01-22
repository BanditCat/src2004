//////////////////////////////////////////////////////
// vis engine source file.
// (c) Jon DuBois 2004
// 10/23/2004
// See license.txt for license details.
// THIS SOFTWARE HAS NO WARRANTY.
////////////////////////////////////////////////////// 
#include <iostream>
#include <cstring>
#include <cctype>
#include <windows.h>
#include "vis/assess.H"
#include "vis/vis.H"
#include "vis/wglext.H"


using namespace std; 



//This is the windows callback for the program
LRESULT CALLBACK wincallback( HWND cbwindowhandle, UINT message, WPARAM wparam, LPARAM lparam );

//All these are global so that wincallback can get to them.
bool buttons[ 3 ];
bool keys[ 256 ];

// Screen info
word swidth = 0, sheight = 0;
bool pfullscreen = false;

// These are used to calculate the average fps.
dword totalframes = 1;
fpnum totaltime = 0;
fpnum lastticks = 0;

// This is the amount given as ticks to the handler if no time has elapsed
static const fpnum smallesttick = 0.001;
// This is the reported fps if fps is infinite
static const fpnum maxfps = 10000;

bool focus;
fpnum pmousedx = 0, pmousedy = 0;
Handler* handler = NULL;

// Windows vars
HDC devicecontext = NULL;	
HGLRC rendercontext = NULL;
HWND windowhandle = NULL;		
HINSTANCE windowinstance;

// True if constructor has completed; stops wincallback from calling handlers prior to construction
bool active = false;
// This gets sets to true after the first tick, it helps the aplication ignore the
// initial mouse movement.
bool running = false;

// This is true if we can do multisampling
bool multisample = false;
// The multisample pixel format.
dword multisampleformat = 0;



Engine::Engine( dword icon ) : pfpsframes( initfpsframes ){
#ifdef DBG
  cout << "Creating engine" << endl;
#endif
  if( exists() )
    throw vis_error( "An attempt was made to double instantiate the engine" );
  exists() = true;

  for( size_t i = 0; i < 3; i++ )
    buttons[ i ] = false;
  for( size_t i = 0; i < 256; i++ )
    keys[ i ] = false;

  windowinstance = GetModuleHandle( NULL );

  WNDCLASS windowclass;

  windowclass.hIcon = LoadIcon( windowinstance, (const char*)icon );
  windowclass.hCursor = LoadCursor(NULL, IDC_ARROW );
  windowclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_DBLCLKS;
  windowclass.hInstance = windowinstance;
  windowclass.hbrBackground = NULL;
  windowclass.lpfnWndProc = (WNDPROC)( wincallback );
  windowclass.cbClsExtra = 0;
  windowclass.cbWndExtra = 0;
  windowclass.lpszMenuName = NULL;
  windowclass.lpszClassName = "OpenGL";

  if ( !RegisterClass( &windowclass ) )
    throw vis_environment_error( "Failed to register the window class" );
	
}
Engine::~Engine( void ){
#ifdef DBG
  cout << "Deconstructing engine" << endl;
#endif
  exists() = false;
  killwindow();

  UnregisterClass( "OpenGL", windowinstance );
  windowinstance = NULL;
  
  screen.close();
}
bool Engine::init( word width, word height, byte bits, byte samples, bool anisotropic, 
		   const char* name, bool fullscreen ){
#ifdef DBG
  cout << "Initializing engine" << endl;
#endif

  if( active )
    throw vis_error( "An attempt was made to double initialize the engine" );

  if( createwindow( width, height, bits, samples, name, fullscreen ) ){
    swidth = width;
    sheight = height;
    pfullscreen = fullscreen;

    View v( swidth, sheight );
    screen.vw() = v;
    screen.panisotropic() = anisotropic;
    screen.init( devicecontext );

    lastticks = GetTickCount();
    active = true;
    return true;
  } else
    return false;  
  
}



bool Engine::tick( void ){
#ifdef DBG
  cout << "Entering Engine::tick" << endl;
#endif
  if( !active )
    throw vis_error( "Attempt to call Engine::tick() with an uninitialized engine" );

  // get timer count.
  fpnum ticks = GetTickCount();
 
  if( !handler )
    throw vis_error( "Attempt to use Engine::tick() without a handler installed" );
  
  pmousedx = 0;
  pmousedy = 0;

  MSG message;

  while( PeekMessage( &message, NULL, 0, 0, PM_REMOVE ) ){
    if( message.message == WM_QUIT )
      return false;
    else{
      TranslateMessage( &message );
      DispatchMessage( &message );
    }
  }
  
  if( !running ){
    pmousedx = 0;
    pmousedy = 0;
    running = true;
  }
  
  totaltime += ticks - lastticks;
  
  if( totaltime )
    handler->tick( totaltime / (fpnum)totalframes );
  else
    handler->tick( smallesttick );

  totaltime -= totaltime / (fpnum)totalframes;
  if( totalframes != pfpsframes )
    totalframes++;

  lastticks = ticks;

  return true;
}
void Engine::sethandler( Handler& hnd ){
  handler = &hnd;
}


bool Engine::button( byte index ) const{ return buttons[ index ]; }
bool Engine::key( byte index ) const{ return keys[ index ]; }
fpnum Engine::mousedx( void ) const{ return pmousedx; } 
fpnum Engine::mousedy( void ) const{ return pmousedy; }
fpnum Engine::fps( void ) const{ return 1000 / ( totaltime / (fpnum)totalframes ); }
dword Engine::fpsframes( void ) const{ return pfpsframes; }
void Engine::fpsframes( dword frames ){ pfpsframes = frames; }
bool Engine::focused( void ) const{ return focus; }
fpnum Engine::aspect( void ) const{  return (fpnum)swidth / (fpnum)sheight; }
dword Engine::width( void ) const{ return swidth; }
dword Engine::height( void ) const{ return sheight; }



LRESULT CALLBACK wincallback( HWND cbwindowhandle, UINT message, WPARAM wparam, LPARAM lparam ){

  if( message == WM_ACTIVATE ){
    if ( !( LOWORD( wparam ) == WA_INACTIVE ) ){
      if( windowhandle )
	SetCapture( windowhandle );
      POINT p;
      p.x = swidth / 2;
      p.y = sheight / 2;
      if( !ClientToScreen( windowhandle, &p ) )
	throw vis_environment_error( "Call to ClientToScreen failed" );
      SetCursorPos( p.x, p.y );
      ShowCursor( false );
      focus = true;				
    } else{
      ReleaseCapture();
      ShowCursor( true );
      focus = false;			
    }
    return 0;	 		
      
    // Prevent screensaver or power saver feature from turning on.
  } else if( ( message == WM_SYSCOMMAND ) && ( ( wparam == SC_SCREENSAVE ) || ( wparam == SC_MONITORPOWER ) ) ){
    return 0;
      
  } else if( message == WM_CLOSE ){
    PostQuitMessage(0);	
    return 0;
      
  } else if( message == WM_SIZE ){
    return 0;
      
      
  } else if( ( message == WM_KEYDOWN ) || ( message == WM_KEYUP ) ){
    KeyEvent key;
    key.symbol = wparam;
    if( message == WM_KEYDOWN ){
      keys[ wparam ] = true;
      key.pressed = true;
    } else{
      keys[ wparam ] = false;
      key.pressed = false;
    }
    if( active )
      handler->keyhandler( key );
#ifdef DBG
      cout << "Handling keystroke in wincallback" << endl;
#endif
      
  } else if( ( message == WM_LBUTTONDOWN ) || ( message == WM_LBUTTONUP ) || ( message == WM_LBUTTONDBLCLK ) ){
    MouseEvent mouse;
    mouse.button = MouseEvent::leftbutton;
    switch( message ){
    case WM_LBUTTONDOWN:
      buttons[ MouseEvent::leftbutton ] = true;
      mouse.type = MouseEvent::pressed;
      break;
    case WM_LBUTTONUP:
      buttons[ MouseEvent::leftbutton ] = false;
      mouse.type = MouseEvent::released;
      break;    
    case WM_LBUTTONDBLCLK:
      buttons[ MouseEvent::leftbutton ] = true;
      mouse.type = MouseEvent::doubleclicked;
      break;
    }
    if( active )
      handler->mousehandler( mouse );
#ifdef DBG
      cout << "Handling left mouse click in wincallback" << endl;
#endif
    return 0;
      
  } else if( ( message == WM_RBUTTONDOWN ) || ( message == WM_RBUTTONUP ) || ( message == WM_RBUTTONDBLCLK ) ){
    MouseEvent mouse;
    mouse.button = MouseEvent::rightbutton;
    switch( message ){
    case WM_RBUTTONDOWN:
      buttons[ MouseEvent::rightbutton ] = true;
      mouse.type = MouseEvent::pressed;
      break;
    case WM_RBUTTONUP:
      buttons[ MouseEvent::rightbutton ] = false;
      mouse.type = MouseEvent::released;
      break;    
    case WM_RBUTTONDBLCLK:
      buttons[ MouseEvent::rightbutton ] = true;
      mouse.type = MouseEvent::doubleclicked;
      break;
    }
    if( active )
      handler->mousehandler( mouse );
#ifdef DBG
      cout << "Handling right mouse click in wincallback" << endl;
#endif
    return 0;
     
  } else if( ( message == WM_MBUTTONDOWN ) || ( message == WM_MBUTTONUP ) || ( message == WM_MBUTTONDBLCLK ) ){
    MouseEvent mouse;
    mouse.button = MouseEvent::middlebutton;
    switch( message ){
    case WM_MBUTTONDOWN:
      buttons[ MouseEvent::middlebutton ] = true;
      mouse.type = MouseEvent::pressed;
      break;
    case WM_MBUTTONUP:
      buttons[ MouseEvent::middlebutton ] = false;
      mouse.type = MouseEvent::released;
      break;    
    case WM_MBUTTONDBLCLK:
      buttons[ MouseEvent::middlebutton ] = true;
      mouse.type = MouseEvent::doubleclicked;
      break;
    }
    if( active )
      handler->mousehandler( mouse );
#ifdef DBG
      cout << "Handling middle mouse click in wincallback" << endl;
#endif
    return 0;
      
  } else if( message == WM_MOUSEMOVE ){
    sword x = LOWORD( lparam );
    sword y = HIWORD( lparam );
    word cx = swidth / 2;
    word cy = sheight / 2;
    if( focus && active && ( ( x != cx ) || ( y != cy ) ) ){
      POINT p;
      p.x = cx;
      p.y = cy;
	
      if( !ClientToScreen( windowhandle, &p ) )
	throw vis_environment_error( "Call to ClientToScreen failed" );
	
      SetCursorPos( p.x, p.y );
      pmousedx = cx - x;
      pmousedy = cy - y;
    }
#ifdef DBG
    cout << "Mouse motion with pmousedx=" << pmousedx << " and pmousedy=" << pmousedy << endl;
#endif
    return 0;
  } 
  
  return DefWindowProc( cbwindowhandle, message, wparam, lparam );
}
void Engine::killwindow( void ){
#ifdef DBG
  cout << "Removing window" << endl;
#endif
  ShowCursor( true ); 
  ReleaseCapture();
  
  if( pfullscreen ){
    ChangeDisplaySettings( NULL, 0 );				
  }

  if( rendercontext ){
    wglMakeCurrent( NULL, NULL );
    wglDeleteContext( rendercontext );
    rendercontext = NULL;
  }
    
  if( devicecontext )
    ReleaseDC( windowhandle, devicecontext );
  
  if( windowhandle ){
    ShowWindow( windowhandle, SW_HIDE );
    DestroyWindow( windowhandle );
    windowhandle = NULL;
  }
 
}
bool Engine::createwindow( word width, word height, byte bits, byte samples, const char* name, bool fullscreen ){
#ifdef DBG
  cout << "Creating window" << endl;
#endif

  dword	exstyle;
  dword style;
  RECT windowrect;

  windowrect.left=(long)0;
  windowrect.right=(long)width;
  windowrect.top=(long)0;
  windowrect.bottom=(long)height;

  if( fullscreen ){
    DEVMODE dmScreenSettings;							       
    memset( &dmScreenSettings, 0, sizeof( dmScreenSettings ) );
    
    dmScreenSettings.dmSize = sizeof( dmScreenSettings );
    dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;
    dmScreenSettings.dmPelsHeight = height;
    dmScreenSettings.dmPelsWidth = width;
    dmScreenSettings.dmBitsPerPel = bits;
    
    
    if( ChangeDisplaySettings( &dmScreenSettings, CDS_FULLSCREEN )!= DISP_CHANGE_SUCCESSFUL ){
      killwindow();
#ifdef DBG
      cout << "Failed to change display settings" << endl;
#endif
      return false;
    }
    
    exstyle = WS_EX_APPWINDOW;
    style = WS_POPUP;
  } else{
    exstyle = WS_EX_APPWINDOW;
    style = WS_OVERLAPPED; 
  }
  
  AdjustWindowRectEx( &windowrect, style, false, exstyle );
  
  if( !( windowhandle = CreateWindowEx(	exstyle, "OpenGL", name, 
					style | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
					CW_USEDEFAULT, CW_USEDEFAULT,
					windowrect.right - windowrect.left, windowrect.bottom - windowrect.top,
					NULL, NULL, windowinstance, NULL ) ) ){
    killwindow();
#ifdef DBG
    cout << "Failed to create window in Engine::createwindow" << endl;
#endif
    return false;
    }
  
  if( !( devicecontext = GetDC( windowhandle ) ) ){
    killwindow();
#ifdef DBG
    cout << "Failed to get device context" << endl;
#endif
    return false;
  }

  dword pixelformat;
  static PIXELFORMATDESCRIPTOR pixelformatd =
    { sizeof( PIXELFORMATDESCRIPTOR ), 1, PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      PFD_TYPE_RGBA, bits, 
      0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
      32, 1, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };
  
  if( multisample )
    pixelformat = multisampleformat;
  else{
    if ( !( pixelformat = ChoosePixelFormat( devicecontext, &pixelformatd ) ) ){
      killwindow();
#ifdef DBG
      cout << "Failed to choose pixel format" << endl;
#endif
      return false;
    }
  }
  
  if( !SetPixelFormat( devicecontext, pixelformat, &pixelformatd ) ){
    killwindow();
#ifdef DBG
      cout << "Failed to set pixel format" << endl;
#endif
    return false;
  }
    
  if ( !( rendercontext = wglCreateContext( devicecontext ) ) ){
    killwindow();
#ifdef DBG
      cout << "Failed to create OpenGL cintext" << endl;
#endif
    return false;
  }

  if( !wglMakeCurrent( devicecontext, rendercontext ) ){
    killwindow();
#ifdef DBG
      cout << "Failed to make OpenGL context active" << endl;
#endif
    return false;
  }
  
  ShowWindow( windowhandle, SW_SHOW );
  SetForegroundWindow( windowhandle );
  SetFocus( windowhandle );
  ShowCursor( false );

  if( !multisample ){
    if( !initmultisample( samples, bits ) ){
#ifdef DBG
      cout << "Failed to initialize multisample" << endl;
#endif
      killwindow();
      return false;
    }
    killwindow();
    return createwindow( width, height, bits, samples, name, fullscreen );
  }

  return true;
}
bool Engine::initmultisample( byte samples, byte bits ){  

  // Get Our Pixel Format
  PFNWGLCHOOSEPIXELFORMATARBPROC wglChoosePixelFormat = 
    (PFNWGLCHOOSEPIXELFORMATARBPROC)wglGetProcAddress("wglChoosePixelFormatARB");	
  if( !wglChoosePixelFormat ){
    multisample = false;
#ifdef DBG
    cout << "Couldn't get address for wglChoosePixelFormatARB" << endl;
#endif
    return false;
  }
  
  int pixelformat;
  int valid;
  UINT numformats;
  float floatattr[] = {0,0};
  
  int intattr[] = { WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		    WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		    WGL_ACCELERATION_ARB, WGL_FULL_ACCELERATION_ARB,
		    WGL_COLOR_BITS_ARB, bits,
		    WGL_ALPHA_BITS_ARB, 8,
		    WGL_DEPTH_BITS_ARB, 32,
		    WGL_STENCIL_BITS_ARB, 8,
		    WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		    WGL_SAMPLE_BUFFERS_ARB, ( samples == 1 ) ? GL_FALSE : GL_TRUE,
		    WGL_SAMPLES_ARB, ( samples == 1 ) ? 0 : samples,
		    0, 0 };
  
  valid = wglChoosePixelFormat( devicecontext, intattr, floatattr, 1, &pixelformat, &numformats );
  if( !valid || !numformats ){
    intattr[ 11 ] = 24;
    valid = wglChoosePixelFormat( devicecontext, intattr, floatattr, 1, &pixelformat, &numformats );
  }
  if( !valid || !numformats ){
    intattr[ 11 ] = 16;
    valid = wglChoosePixelFormat( devicecontext, intattr, floatattr, 1, &pixelformat, &numformats );
  }

  if( valid && numformats >= 1 ){
    multisample = true;
    multisampleformat = pixelformat;	
    return true;
  } else{
#ifdef DBG
    cout << "Failed to choose OpenGL pixel format" << endl;
#endif
    return false;
  }
}
