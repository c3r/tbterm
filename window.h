#pragma once
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <SDL2/SDL.h>
#include <signal.h>
#include <vector>
#include <cstdint>
#include <memory>
#include "textbuffer.h"
#include "console.h"

struct Context;

class Window {
	private:
		Context *_ctx = nullptr;
		SDL_Window *_window = nullptr;
		SDL_Surface *_surface = nullptr;
	public:
		Window(Context *ctx) : _ctx(ctx) {}
		~Window();

		bool InitSDL();
		void QuitSDL();

		bool Create();
		void Destroy();

                bool HandleEvents();
};
