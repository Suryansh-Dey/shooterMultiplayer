SDL_Renderer *createWindow(int SCREEN_WIDTH, int SCREEN_HEIGHT)
{
	SDL_Window *window = SDL_CreateWindow("Shooter AI", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer *s = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderClear(s);
	return s;
}
#ifdef __ANDROID__
#include "mobileInputManager.cxx"
#elif defined(__APPLE__)
#include "mobileInputManager.cxx"
#else
#include "computerInputManager.cxx"
#endif