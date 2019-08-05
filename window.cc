#include "window.h"
#include "context.h"

bool TermWindow::InitX11() {
    static bool X11_initialized; // not multithreading safe.

    if (X11_initialized) 
    	return true; 

    XOpenDisplay(NULL);

    if (_display == nullptr) 
    {
	fprintf(stderr, "XOpenDisplay: error\n"); 
    	return false;
    }
    
    WM_DELETE_WINDOW = XInternAtom(_display,"WM_DELETE_WINDOW",false);
    WM_SIZE_HINTS = XInternAtom(_display,"WM_SIZE_HINTS",false);
    WM_NORMAL_HINTS = XInternAtom(_display, "WM_NORMAL_HINTS",false);
    _NET_WM_ALLOWED_ACTIONS = XInternetAtom(_display, "_NET_WM_ALLOWED_ACTIONS",false);
    _NET_WM_ACTION_CLOSE = XInternetAtom(_display, "_NET_WM_ACTION_CLOSE",false);
    _NET_WM_ACTION_MINIMIZE = XInternetAtom(_display, "_NET_WM_ACTION_MINIMIZE",false);
    _NET_WM_ACTION_MOVE	= XInternetAtom(_display, "_NET_WM_ACTION_MOVE",false); 

    _screen = DefaultScreen(_display);
    _visual = DefaultVisual(_display, _screen);

    X11_initialized = true;
    return true;
}

void TermWindow::QuitX11() 
{ 
    XCloseDisplay(_display);
}

void TermWindow::Destroy() 
{ 
    if (_window != nullptr)
	XDestroyWindow(_display, _window);
}

TermWindow::~TermWindow() { Destroy(); }

bool TermWindow::Create() 
{
    int xpos = 10;
    int ypos = 10;
    int border_width = 1;
    unsigned long border = BlackPixel(_display, _screen);
    unsigned long background = BlackPixel(_display, _screen);

    _window = XCreateSimpleWindow( 
		_display,
		RootWindow(_display, _screen),
		xpos,
		ypos,
		kStartWidth, 
		kStartHeight, 
		border_width
		border, 
		background
	    );
    	    
/*
    if (_window == nullptr) 
    { 
	fprintf(stderr, "XCreateSimpleWindow: error\n"); 
	return false; 
    }

*/
    XStoreName(_display, _window, "tbterm");
    XSetWMProtocols(_display, _window, &WM_DELETE_WINDOW, 1);
    XSetSelectInput(_display, _window, KeyPressMask | ExposureMask);

    XSizeHints xsh;
    xsh.flags = PMinSize | PMaxSize;
    xsh.min_width = xsh.max_width = kStartWidth;
    xsh.min_height = xsh.max_height = kStartHeight;
    XSetWMSizeHints(_display, _window, &xsh, WM_SIZE_HINTS);
    XSetWMNormalHints(_display, win, &xsh);

    const Atom allowed[] = {
	_NET_WM_ACTION_CLOSE,
	_NET_WM_ACTION_MINIMIZE,
	_NET_WM_ACTION_MOVE
    };

    XChangeProperty(
	_display, 
	_window, 
	_NET_WM_ALLOWED_ACTIONS, 
	XA_ATOM, 
	32,
	PropertyNewValue,
	(unsigned char*) &allowed,
	3
    );

    XMapWindow(_display, _window);
    XFlush(_display);

    _frame = new uint32_t[kStartWidth * kStartHeight];

    _surface = XCreateImage(
		_display, 
		_visual, 
		/* depth = */  24, 
		/* format = */ ZPixmap, 
		/* offset = */ 0,
		(char*)frame,
		kStartWidth,
		kStartHeight,
		32,
		0
	    );


    //if (_surface == nullptr) 
    //{ 
    //	fprintf(stderr, "SDL_GetTermWindowSurface: error\n"); 
    //	return false; 
    //}

    ResizeConsoles();
    return true;
}

void TermWindow::ResizeConsoles() 
{
    uint32_t w = (uint32_t) _surface->w;
    uint32_t h = (uint32_t) _surface->h;
    for (auto& console : _ctx->consoles) 
    {
	console->ResizeTextBuffer(w, h);
	console->HandleSurfaceChange(_surface);
    }
}

bool TermWindow::HandleEvents() 
{
    SDL_PumpEvents();
    SDL_Event ev;

    while (SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0)
    {
	if (ev.type == SDL_QUIT) { return false; }
	if (ev.type == SDL_WINDOWEVENT) {
	    // TermWindow size has changed - we have to reinitialize the SDL_Surface here.
	    if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) 
	    {	
		_surface = SDL_GetTermWindowSurface(_window);
		if (_surface == nullptr) 
		{ 
		    fprintf(stderr, "SDL_GetTermWindowSurface: error\n"); 
		    return false; 
		}
		continue;
	    }

	    // Make sure that every closing event of the window will be pushed
	    // to the SDL event queue
	    if (ev.window.event == SDL_WINDOWEVENT_CLOSE) 
	    {
		SDL_Event quit_ev;
		quit_ev.type = SDL_QUIT;
		SDL_PushEvent(&quit_ev);
		continue;
	    }
	}
    }
    return true;
}


void TermWindow::RedrawTermWindowIfConsoleActive(Console *console)
{
    //TOOD: check if this console is actually active
    SDL_UpdateTermWindowSurface(_window);
}
