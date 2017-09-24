#include "Game.h"
#include <SDL.h>



Game::Game()//constructer
{
}


Game::~Game()//deconstructer
{
}

void Game::init(const char * title, int xpos, int ypos, int width, int height, bool fullscreen) {
	int flags = 0;
	if (fullscreen) {
		flags = SDL_WINDOW_FULLSCREEN;
	}

	//intialize
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {
		std::cout << "Initialized" << std::endl;
		window = SDL_CreateWindow(title, xpos, ypos, width, height, flags);


		if (window) {
			std::cout << "Window created" << std::endl;
		}
		renderer = SDL_CreateRenderer(window, -1, 0);
		if (renderer) {
			SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
			std::cout << "Rendered" << std::endl;
		}
		isRunning = true;
	}
	else {
		isRunning = false;
	}
}

void Game::eventhandle() {
	SDL_Event event;
	SDL_PollEvent(&event);
	switch (event.type) {
	case SDL_QUIT:
		isRunning = false;
		break;
	default:
		break;
	}
}

void Game::update() {
	count++;
	std::cout << count << std::endl;
}

void Game::render() {
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
}

void Game::clean() {
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	std::cout << "Cleaned" << std::endl;
}
