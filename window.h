#pragma once
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <errno.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <vector>
#include <cstdint>
#include <memory>

#include "textbuffer.h"
#include "console.h"

struct Context;

class TermWindow {
    private:
	Context *_ctx = nullptr;
	Window *_window = nullptr;
	Display *_display = nullptr;
	Visual *_visual = nullptr;
	XImage *_surface = nullptr;
	uint32_t *_frame = nullptr;

	Atom WM_DELETE_WINDOW;
	Atom WM_SIZE_HINTS;
	Atom WM_NORMAL_HINTS;
	Atom _NET_WM_ALLOWED_ACTIONS;
	Atom _NET_WM_ACTION_CLOSE;
	Atom _NET_WM_ACTION_MINIMIZE;
	Atom _NET_WM_ACTION_MOVE;

	const int kDefaultWidth = 640;
	const int kDefaultHeight = 480;

    public:
	TermWindow(Context *ctx) : _ctx(ctx) {}
	~TermWindow();

	bool InitX11();
	void QuitX11();

	bool Create();
	void Destroy();

	bool HandleEvents();
	void ResizeConsoles();

	void RedrawTermWindowIfConsoleActive(Console *console);
};
