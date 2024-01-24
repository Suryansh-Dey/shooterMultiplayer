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
    return abs(rect.x - rect.x) < rect.w / 2 and abs(rect.y - rect.y) < rect.h / 2;
}

class Menu
{
    const int SCREEN_WIDTH, SCREEN_HEIGHT;
    uint32_t time = 0;
    ButtonIcon cancelIcon, exitIcon, joinRandomIcon, generateCodeIcon, joinByCodeIcon;
    Shooter particleEffects;
    void pressButtons(int x, int y, Client& client);

public:
    Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT, std::unordered_map<std::string, SDL_Texture *> images);
    bool run(Client &client, SDL_Renderer *renderer);
};
Menu::Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT, std::unordered_map<std::string, SDL_Texture *> images) : SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), cancelIcon(images["cancel"], 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), exitIcon(images["exit"], 0.9 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), joinRandomIcon(images["joinRandom"], 0.1 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), generateCodeIcon(images["generateCode"], 0.1 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), joinByCodeIcon(images["joinByCode"], 0.1 * SCREEN_WIDTH, 0.3 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), particleEffects(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 10, Game::shooterImages)
{
}
void Menu::pressButtons(int x, int y, Client &client)
{
    if(joinRandomIcon.isPressed(x,y))
        client.joinRandom();
}
bool Menu::run(Client &client, SDL_Renderer *renderer)
{
    while (true)
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
                break;
            case SDL_MOUSEMOTION:
                particleEffects.sprinkleSnow(mouseX, mouseY, 10, 10, 90);
                break;
            case SDL_KEYDOWN:
                pressButtons(mouseX,mouseY,client);
            }
        }
        SDL_RenderClear(renderer);
        particleEffects.sprinkler.render(renderer, time);
        SDL_RenderCopy(renderer, Game::backgroundImage, NULL, NULL);
        SDL_RenderPresent(renderer);
        FPS_manager(Game::FRAME_GAP);
        time++;
    }
}
