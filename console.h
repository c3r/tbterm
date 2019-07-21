#pragma once

#include <atomic>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <SDL2/SDL.h>
#include <vector>
#include <cstdint>
#include <thread>
#include <unistd.h>
#include <fcntl.h>
#include "textbuffer.h"

class Console { 

    private:
	void ReaderWorker();

	std::atomic<bool> _end_threads{false};

	SDL_Surface *_surface = nullptr;
	TextBuffer _buffer;
	pid_t _pid = -1;
	int _master = -1;
	std::unique_ptr<std::thread> _read_th;

    public:
	void ResetPid();
	pid_t GetPid() const;
	int GetMaster();
	void HandleSurfaceChange(SDL_Surface *surface);
	void ResizeTextBuffer(uint32_t w, uint32_t h);
	bool SpawnChild();
	void CloseMaster();
};

