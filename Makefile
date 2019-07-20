all:
	g++ -Wall -Wextra -o tbterm \
		tbterm.cc \
		textbuffer.cc \
		console.cc \
		window.cc \
		-lSDL2 

test: all
	./tbterm

