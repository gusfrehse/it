#include "window.h"

#include <SDL2/SDL.h>
#include <cstdio>

window::window(int width, int height , char const *name) {
	m_window = SDL_CreateWindow(
			name,
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width,
			height,
			SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL
			);

	if (m_window == NULL) {
		//log
		std::fprintf(stderr, "Window creation failed\n");
		exit(1);
	}
}

window::~window() {
	SDL_DestroyWindow(m_window);
}
