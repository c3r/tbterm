#include <algorithm>
#include <poll.h>
#include "console.h"

// TODO: Handle console control codes
void Console::ProcessOutput(uint8_t *data, size_t size) { for (size_t i = 0; i < size; i++) { _buffer.InsertCharacter(data[i]); } }
void Console::HandleSurfaceChange(SDL_Surface *surface) { _surface = surface; }

void Console::ResizeTextBuffer(uint32_t w, uint32_t h) {
    unsigned int columns = std::max(w / _char_width, 1U);
    unsigned int lines = std::max(h / _char_height, 1U);
    _buffer.Resize(columns, _scrollback_lines);
    // TODO: send signal to child
}

pid_t Console::GetPid() const { return _pid; }
void Console::ResetPid() { _pid = -1; }

bool Console::SpawnChild() {

    _master = getpt();
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

    _pid = fork();

    if (_pid == -1) 
    {
	perror("fork");
	close(_master);
	close(slave);
	return false;
    }

    if (_pid == 0) {
	printf("I'm child! (%d)\n", getpid());
	close(0); // stdin
	close(1); // stdout
	close(2); // stderr
	close(_master);
	_master = -1;

	dup2(slave, 0);
	dup2(slave, 1);
	dup2(slave, 2);

	close(slave);

	if (execl("/bin/bash", "/bin/bash", nullptr) == -1) {
	    abort();
	}
    }

    printf("I'm master! (%d)\n", getpid());
    close(slave);
    _read_th = std::make_unique<std::thread>(&Console::ReaderWorker, this);

    return true;
}

void Console::ReaderWorker() {
    uint8_t buf[4096];
    pollfd fds[] = {{ _master, POLLIN | POLLPRI | POLLRDHUP | POLLERR, 0 /* ignored */ }};

    while (!_end_threads.load()) 
    {
	int ret = poll(fds, 1, 50 /* ms */);
	if (ret == -1) { perror("poll"); break; }
	if (ret ==  0) { continue; } // Timeout.
	const int revents = fds[0].revents;
	if ((revents & POLLIN)) 
	{
	    ssize_t buf_read = read(_master, buf, sizeof(buf));
	    if (buf_read == -1) { perror("ReaderWorker"); }
	    ProcessOutput(buf, (size_t)buf_read);
	} 
	else fprintf(stderr, "poll revents == %.x\n", revents);
    }
}

void Console::CloseMaster() {
    _end_threads.store(true);
    _read_th->join();
    printf("master: %d\n", _master);
    close(_master);
    _master = -1;
}
