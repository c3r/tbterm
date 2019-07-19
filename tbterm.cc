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

int main() {

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
		// Child process. ==============================================
		
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

		if (execl("/bin/bash", "/bin/bash", nullptr) == -1) {
			abort();
		}
	}

	// Parent process. =================================================
	close(slave);
	waitpid(pid, nullptr, 0);
	puts("Bye bye.");
}
