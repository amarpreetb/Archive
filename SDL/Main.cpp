//#include <stdio.h>
#include <SDL.h>
#include <Game.h>

Game *game = nullptr;

int main(int argc, char* argv[]) {

	game = new Game();

	//initialize
	//title, width, height, postion and size
	game->init("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, false);

	//running
	while (game-> running()) {
		game->eventhandle();
		game->update();
		game->render();

	}

	//quit
	game->clean();


	return 0;
}
