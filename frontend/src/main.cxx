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
int SCREEN_WIDTH, SCREEN_HEIGHT;
std::string serverURL;
int main(int argc, char *argv[])
{
	if (argc == 2)
		serverURL = argv[1];
	else
		serverURL = "http://localhost:3000";
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	SDL_Renderer *renderer = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game::loadResources(renderer, "../resources");

	while (true)
	{
		Menu menu(SCREEN_WIDTH, SCREEN_HEIGHT, renderer);
		Client client(serverURL, SCREEN_WIDTH, SCREEN_HEIGHT);
		Menu::Result result = menu.run(client);
		if (result == Menu::Result::EXIT)
			break;
		else if (result == Menu::Result::CANCELLED)
			continue;
		Game game(SCREEN_WIDTH, SCREEN_HEIGHT, renderer, client);
		if (game.run())
			break;
	}
	return 0;
}
