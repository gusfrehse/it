#ifndef IT_WINDOW_H
#define IT_WINDOW_H

#include <SDL2/SDL.h>

class window
{
	SDL_Window *m_window;

public:
	window(int width, int height, char const *name);
	~window();
};

#endif
