#include "window.h"
#include "context.h"

bool Window::InitSDL() {
    static bool SDL_initialized; // not multithreading safe.
    if (SDL_initialized) { return true; }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_EVENTS) != 0) { fprintf(stderr, "SDL_Init: error\n"); return false; } 
    SDL_initialized = true;
    return SDL_initialized;
}

void Window::QuitSDL() { SDL_Quit(); }
Window::~Window() { Destroy(); }
void Window::Destroy() { if (_window != nullptr) SDL_DestroyWindow(_window); }

bool Window::Create() 
{
    _window = SDL_CreateWindow( "tbterm",
				SDL_WINDOWPOS_CENTERED,
				SDL_WINDOWPOS_CENTERED,
				640, 480,
				SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (_window == nullptr) 
    { 
    	fprintf(stderr, "SDL_CreateWindow: error\n"); 
    	return false; 
    }

    _surface = SDL_GetWindowSurface(_window);
    if (_surface == nullptr) 
    { 
    	fprintf(stderr, "SDL_GetWindowSurface: error\n"); 
    	return false; 
    }

    ResizeConsoles();
    return true;
}

void Window::ResizeConsoles() 
{
    uint32_t w = (uint32_t) _surface->w;
    uint32_t h = (uint32_t) _surface->h;
    for (auto& console : _ctx->consoles) 
    {
	console->ResizeTextBuffer(w, h);
	console->HandleSurfaceChange(_surface);
    }
}

bool Window::HandleEvents() 
{
    SDL_PumpEvents();
    SDL_Event ev;

    while (SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0)
    {
	if (ev.type == SDL_QUIT) { return false; }
	if (ev.type == SDL_WINDOWEVENT) {
	    // Window size has changed - we have to reinitialize the SDL_Surface here.
	    if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) 
	    {	
		_surface = SDL_GetWindowSurface(_window);
		if (_surface == nullptr) 
		{ 
		    fprintf(stderr, "SDL_GetWindowSurface: error\n"); 
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
