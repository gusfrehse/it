// SDL2 Hello, World!
// This should display a white screen for 2 seconds
// compile with: clang++ main.cpp -o hello_sdl2 -lSDL2
// run with: ./hello_sdl2
#include <SDL2/SDL.h>
#include <cstdio>
#include <ctime>
#include <cstdlib>
#include <vector>

#include "window.h"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define BLOCK_SIZE 512

std::vector<char> read_file(char *path) {
	int size = 0;
	int buf_cap = 0;
	char c;
	std::vector<char> buffer;

	FILE *f;

	f = fopen(path, "r");

	if (!f) {
		std::fprintf(stderr, "Couldn't open file '%s'\n", path);
		std::exit(1);
	}

	while ((c = std::getc(f)) != EOF) {
		buffer.push_back(c);
	}

	fclose(f);

	return buffer;
}


int main(int argc, char* argv[]) {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		std::fprintf(stderr, "could not initialize sdl2: %s\n", SDL_GetError());
		return 1;
	}

	std::vector buffer = read_file(argv[1]);

	Window win(SCREEN_WIDTH, SCREEN_HEIGHT, "it");

	SDL_Quit();
	return 0;
}
