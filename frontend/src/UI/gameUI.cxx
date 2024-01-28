#ifdef __ANDROID__
#include "mobileInputManager.cxx"
SDL_Renderer *createWindow(int &SCREEN_WIDTH, int &SCREEN_HEIGHT)
{
	SDL_Window *window = SDL_CreateWindow("Magic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN);
	SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderClear(renderer);
	return renderer;
}
#elif defined(__APPLE__)
#include "mobileInputManager.cxx"
SDL_Renderer *createWindow(int &SCREEN_WIDTH, int &SCREEN_HEIGHT)
{
	SDL_Window *window = SDL_CreateWindow("Magic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN);
	SDL_GetWindowSize(window, &SCREEN_WIDTH, &SCREEN_HEIGHT);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SDL_RenderClear(renderer);
	return renderer;
}
#else
#include "computerInputManager.cxx"
SDL_Renderer *createWindow(int &SCREEN_WIDTH, int &SCREEN_HEIGHT)
{
	SDL_DisplayMode dm;
	if (SDL_GetDesktopDisplayMode(0, &dm) != 0)
	{
		SDL_Quit();
		exit(1);
	}

	// Set up SDL window and renderer
	SDL_Window *window = SDL_CreateWindow(
		"Magic", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, dm.w, dm.h,
		SDL_WINDOW_FULLSCREEN | SDL_WINDOW_OPENGL);
	SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	SCREEN_WIDTH = dm.w;
	SCREEN_HEIGHT = dm.h;
	SDL_RenderClear(renderer);
	return renderer;
}
#endif