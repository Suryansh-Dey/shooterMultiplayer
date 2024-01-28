#include <cmath>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "functions.cxx"
#include "animation.cxx"
#include "shooter.cxx"
#include "networking.cxx"
#include "UI/gameUI.cxx"
#include "game.cxx"
#include "UI/menuUI.cxx"
int SCREEN_WIDTH = 800, SCREEN_HEIGHT = 500;
bool quit = false;

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_Renderer *renderer = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game::loadResources(renderer, "../resources");

	while (!quit)
	{
		Menu menu(SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
		Client client("http://localhost:3000", SCREEN_WIDTH, SCREEN_HEIGHT);
		quit = menu.run(client);
		if (quit)
			break;
		else if (not client.getId())
			continue;
		Game game(SCREEN_WIDTH, SCREEN_HEIGHT, renderer, client);
		quit = game.run();
		std::cout << "Match finished\n";
	}
	return 0;
}
