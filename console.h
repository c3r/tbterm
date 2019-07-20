#pragma once
#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif
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
#include "textbuffer.h"

class Console { 
	
	private:
		SDL_Surface *_surface = nullptr;
		TextBuffer _buffer;
		pid_t _pid;
		int _master = -1;

	public:
	        void ResetPid();
		pid_t GetPid() const;
		void HandleSurfaceChange(SDL_Surface *surface);
		void ResizeTextBuffer(uint32_t w, uint32_t h);
		bool SpawnChild();
		void CloseMaster();
};

