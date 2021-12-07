#ifndef IT_WINDOW_H
#define IT_WINDOW_H

#include <SDL2/SDL.h>

class Window
{
public:
	int m_width, m_height;

private:
	SDL_Window *m_window;

public:
	Window(int width, int height, char const *name);
	~Window();
};

#endif
