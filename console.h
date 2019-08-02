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

struct Context;

class Console {

    private:
	void ReaderWorker();
	void ProcessOutput(uint8_t *data, size_t size);

	std::atomic<bool> _end_threads{false};

	SDL_Surface *_surface = nullptr;
	TextBuffer _buffer;
	pid_t _pid = -1;
	int _master = -1;
	std::unique_ptr<std::thread> _read_th;

	uint32_t _char_width = 16;
	uint32_t _char_height = 16;
	uint32_t _lines = 1; // Actual console size.
	uint32_t _scrollback_lines = 1000;
	Context* _ctx = nullptr;  // Console is not the owner of this object.

    public:
	Console(Context* ctx) : _ctx(ctx) {}
	void ResetPid();
	pid_t GetPid() const;
	int GetMaster();
	void HandleSurfaceChange(SDL_Surface *surface);
	void ResizeTextBuffer(uint32_t w, uint32_t h); //Pixels
	bool SpawnChild();
	void CloseMaster();
	void UpdateSurface();
};
