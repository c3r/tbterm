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
#include "window.h"
#include "console.h"

struct Context {
	std::vector<std::unique_ptr<Console>> consoles;
	std::unique_ptr<TermWindow> wnd;
};
