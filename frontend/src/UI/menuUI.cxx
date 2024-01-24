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
    return abs(rect.x - x) < rect.w / 2 and abs(rect.y - y) < rect.h / 2;
}

class Menu
{
    const int SCREEN_WIDTH, SCREEN_HEIGHT;
    uint32_t time = 0;
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
    while (!client.getMatch().active)
    {
        SDL_Event event;
        int mouseX, mouseY;
        SDL_GetMouseState(&mouseX, &mouseY);
        mouseX *= SCREEN_WIDTH;
        mouseY *= SCREEN_HEIGHT;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                particleEffects.sprinkleFire(mouseX, mouseY, 20, 10);
                pressButtons(mouseX, mouseY, client);
                break;
            case SDL_MOUSEMOTION:
                particleEffects.sprinkleSnow(mouseX, mouseY, 10, 10, 90);
                break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, Game::backgroundImage, NULL, NULL);
        particleEffects.sprinkler.render(renderer, time);
        joinByCodeIcon.render(renderer);
        SDL_RenderPresent(renderer);
        FPS_manager(Game::FRAME_GAP);
        time++;
    }
    return false;
}
