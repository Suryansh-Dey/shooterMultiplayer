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
#include "UI/UI.cxx"
#include "game.cxx"
int SCREEN_WIDTH, SCREEN_HEIGHT;

int main(int argc, char* argv[])
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Renderer *s = createWindow(SCREEN_WIDTH, SCREEN_HEIGHT);
	
	Game game(SCREEN_WIDTH,SCREEN_HEIGHT, s);
	bool success = game.main();
	return !success;
}