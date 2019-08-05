all:
	g++ -g -Wall -Wextra -o tbterm \
		tbterm.cc \
		textbuffer.cc \
		console.cc \
		window.cc \
		-lpthread -lX11

clang-all:
	clang++-8 -Wall -Wextra -o tbterm \
		tbterm.cc \
		textbuffer.cc \
		console.cc \
		window.cc \
		-lSDL2 -lpthread 
test: all
	./tbterm

