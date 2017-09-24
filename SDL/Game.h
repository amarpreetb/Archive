#ifndef Game_hpp
#define Game_hpp
#include <SDL.h>
//#undef main
#include <iostream>

class Game
{
public:
	Game();//contructer
	~Game();//decontructer

	void init(const char* title, int xpos, int ypos, int width, int height, bool fullscreen);
	
	void eventhandle();
	void update();
	void render();
	void clean();

	bool running() {
		return isRunning;
	}

private:
	int count = 0;
	bool isRunning;
	SDL_Window *window;
	SDL_Renderer *renderer;
};

#endif
