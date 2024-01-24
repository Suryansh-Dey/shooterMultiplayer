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
	SDL_Renderer *s = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	Game::loadResources(s, "../resources");

	while (!quit)
	{
		Game game(SCREEN_WIDTH, SCREEN_HEIGHT, s, "http://localhost:3000");
		quit = game.run();
		std::cout << "Match finished\n";
	}
	return 0;
}