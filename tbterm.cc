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
#include <memory>
#include "textbuffer.h"
#include "console.h"
#include "context.h"
#include "window.h"

int main() {

    Context ctx;	
    ctx.wnd = std::make_unique<Window>(&ctx);
    if (!ctx.wnd->InitSDL() || !ctx.wnd->Create()) { return 1; }

    ctx.consoles.emplace_back( std::make_unique<Console>() );
    if (!ctx.consoles[0]->SpawnChild()) { return 2; }

    bool end = false;
    while (!end) {
        for (auto& console : ctx.consoles) {
            int status;
            if (waitpid(console->GetPid(), &status, WNOHANG) != 0) {
                console->ResetPid();
                end = true;
                break;
            }
        }

        if (!ctx.wnd->HandleEvents()) {
            end = true;
            break;
        }

    }

    ctx.wnd->Destroy();
    ctx.wnd->QuitSDL();

    for (auto& console : ctx.consoles) {
        // This should force the child to exit.
        //console->CloseMaster();
    }

    // Keep this as a separate loop.
    for (auto& console : ctx.consoles) {
    	console->CloseMaster();
    	pid_t pid = console->GetPid();
        if (pid != -1) { 
	    puts("Waiting for child to die...");
            waitpid(pid, nullptr, 0); // Should not be WNOHANG here...
        }
    }

    puts("Bye bye.");
}
