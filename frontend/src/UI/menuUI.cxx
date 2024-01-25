#include "buttons.cxx"
class ButtonIcon : public Icon
{
public:
    ButtonIcon(SDL_Texture *image, int x, int y, int w, int h);
    inline bool isPressed(int x, int y);
};
ButtonIcon::ButtonIcon(SDL_Texture *image, int x, int y, int w, int h) : Icon(image, x, y, w, h)
{
}
bool ButtonIcon::isPressed(int x, int y)
{
    return x - rect.x < rect.w and y - rect.y < rect.h;
}

class Menu
{
    const int SCREEN_WIDTH, SCREEN_HEIGHT;
    ButtonIcon cancelIcon, exitIcon, joinRandomIcon, generateCodeIcon, joinByCodeIcon;
    Shooter particleEffects;
    void pressButtons(int x, int y, Client &client);

public:
    Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT);
    bool run(Client &client, SDL_Renderer *renderer);
};
Menu::Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT) : SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), cancelIcon(Game::buttonImages["cancel"], 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), exitIcon(Game::buttonImages["exit"], 0.9 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), joinRandomIcon(Game::buttonImages["joinRandom"], 0.1 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), generateCodeIcon(Game::buttonImages["generateCode"], 0.1 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), joinByCodeIcon(Game::buttonImages["joinByCode"], 0.1 * SCREEN_WIDTH, 0.3 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), particleEffects(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 10, Game::shooterImages)
{
}
void Menu::pressButtons(int x, int y, Client &client)
{
    if (joinRandomIcon.isPressed(x, y))
        client.joinRandom();
}
bool Menu::run(Client &client, SDL_Renderer *renderer)
{
    SDL_SetRelativeMouseMode(SDL_FALSE);
    while (true)
    {
        SDL_Event event;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                particleEffects.sprinkleFire(mouseX, mouseY, 30, 30, 0, 0);
                pressButtons(mouseX, mouseY, client);
                break;
            case SDL_MOUSEMOTION:
                particleEffects.sprinkleSnow(mouseX, mouseY, 5, 10, 90, 10);
                break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, Game::backgroundImage, NULL, NULL);
        joinRandomIcon.render(renderer);
        particleEffects.sprinkler.render(renderer, particleEffects.time);
        SDL_RenderPresent(renderer);
        FPS_manager(Game::FRAME_GAP);
        particleEffects.time++;
        if (client.getId())
        {
            if (client.getMatch().active)
                break;
        }
    }
    return false;
}
