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

int main() {

	// Master / slave pseudoterminal configuration
	// interprocess communication 
	// ====================================================================

	int master = getpt();
	if (master == -1) {
		perror("getpt");
		return 1;
	}

	char slave_path[256]{};
	if (ptsname_r(master, slave_path, sizeof(slave_path)) != 0) {
		perror("ptsname_r");
		close(master);
		return 2;
	}

	if (grantpt(master) != 0) {
		perror("grantpt");
		close(master);
		return 3;
	}


	if (unlockpt(master) != 0) {
		perror("unlockpt");
		close(master);
		return 4;
	}

	int slave = open(slave_path, O_RDWR);
	if (slave < 0) {
		perror("open");
		close(master);
		return 3;
	}

	// duplicate the process
	pid_t pid = fork();
	if (pid == -1) {
		perror("fork");
		return 5;
	}

	if (pid == 0) {
		// Child process. 
		// ====================================================================

		// Zamykamy niepotrzebne dekryptory - mamy otwarte bo zdupliko
		// wany proces jest.
		close(0); // stdin
		close(1); // stdout
		close(2); // stderr
		close(master);

		dup2(slave, 0);
		dup2(slave, 1);
		dup2(slave, 2);

		close(slave);

		// start desired shell 
		if (execl("/bin/bash", "/bin/bash", nullptr) == -1) {
			abort();
		}
	}

	// Parent process. 
	// ====================================================================
	close(slave);

	// window creation 
	// ====================================================================
	if (SDL_Init(SDL_INIT_VIDEO |
				 SDL_INIT_TIMER |
				 SDL_INIT_EVENTS) != 0) {
		fprintf(stderr, "SDL_Init: error\n");
		return 1;
	}

	SDL_Window *window = SDL_CreateWindow(
		"tbterm",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 480,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	if (window == nullptr) {
		fprintf(stderr, "SDL_CreateWindow: error\n");
		return 1;
	}

	SDL_Surface *surface = SDL_GetWindowSurface(window);

	if (surface == nullptr) {
		fprintf(stderr, "SDL_GetWindowSurface: error\n");
		return 1;
	}
	//event loop
	bool end = false;
	while (!end) {
		int status;
		if (waitpid(pid, &status, WNOHANG) != 0) {
			pid = -1;
			end = true;
			break;
		}

		SDL_PumpEvents();
		SDL_Event ev;
		while (SDL_PeepEvents(&ev, 1, SDL_GETEVENT, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0) {
			if (ev.type == SDL_QUIT) {
				end = true;
				break;
			}
			if (ev.type == SDL_WINDOWEVENT) {

				// Window size has changed - we have to reinitialize the SDL_Surface here.
				if (ev.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
					surface = SDL_GetWindowSurface(window);
					if (surface == nullptr) {
						fprintf(stderr, "SDL_GetWindowSurface: error\n");
						end = true;
					}
					continue;
				}

				// Make sure that every closing event of the window will be pushed
				// to the SDL event queue 
				if (ev.window.event == SDL_WINDOWEVENT_CLOSE) {
					SDL_Event quit_ev;
					quit_ev.type = SDL_QUIT;
					SDL_PushEvent(&quit_ev);
					continue;
				}
			}
		}
	}



	// destroy SDL window
	SDL_DestroyWindow(window);
	SDL_Quit();

	close(master);
	if (pid != -1) { 
		// Wait for the child to die.
		waitpid(pid, nullptr, 0); 
	}
	puts("Bye bye.");
}
