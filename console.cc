#include "console.h"

void Console::HandleSurfaceChange(SDL_Surface *surface) {

}

void Console::ResizeTextBuffer(uint32_t w, uint32_t h) {

}

pid_t Console::GetPid() const {
    return _pid;
}

void Console::ResetPid() {
    _pid = -1;
}

void Console::CloseMaster() {
    close(_master);
}

bool Console::SpawnChild() {

    int _master = getpt();
    if (_master == -1) {
        perror("getpt");
        return false;
    }

    char slave_path[256]{};
    if (ptsname_r(_master, slave_path, sizeof(slave_path)) != 0) {
        perror("ptsname_r");
        close(_master);
        return false;
    }

    if (grantpt(_master) != 0) {
        perror("grantpt");
        close(_master);
        return false;
    } 


    if (unlockpt(_master) != 0) {
        perror("unlockpt");
        close(_master);
        return false;
    }

    int slave = open(slave_path, O_RDWR);
    if (slave < 0) {
        perror("open");
        close(_master);
        return false;
    }

    // duplicate the process
    _pid = fork();
    if (_pid == -1) {
        perror("fork");
        close(_master);
        close(slave);
        return false;
    }

    if (_pid == 0) {
        // Child process. 
        // ====================================================================

        // Zamykamy niepotrzebne dekryptory - mamy otwarte bo zdupliko
        // wany proces jest.
        close(0); // stdin
        close(1); // stdout
        close(2); // stderr
        close(_master);
        _master = -1;

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

    return true;
}
