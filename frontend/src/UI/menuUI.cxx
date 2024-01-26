#include "buttons.cxx"
class ButtonIcon : public Icon
{
    Icon name;
    bool active;

public:
    ButtonIcon(SDL_Renderer *renderer, std::string name, int x, int y, int w, int h, bool active = true);
    inline bool isPressed(int x, int y);
    inline void disable();
    inline void enable();
    void render(SDL_Renderer *renderer, uint8_t alpha = 255);
};
ButtonIcon::ButtonIcon(SDL_Renderer *renderer, std::string name, int x, int y, int w, int h, bool active) : Icon(Game::buttonImages["button"], x, y, w, h), name(renderer, name, Game::font, {255, 255, 0, 255}, x, y), active(active)
{
}
bool ButtonIcon::isPressed(int x, int y)
{
    return active and x - rect.x < rect.w and x - rect.x > 0 and y - rect.y < rect.h and y - rect.y > 0;
}
void ButtonIcon::disable()
{
    this->active = false;
}
void ButtonIcon::enable()
{
    this->active = true;
}
void ButtonIcon::render(SDL_Renderer *renderer, uint8_t alpha)
{
    if (!this->active)
        return;
    Icon::render(renderer, alpha);
    name.render(renderer, alpha);
}

class Menu
{
    const int SCREEN_WIDTH, SCREEN_HEIGHT;
    ButtonIcon cancelIcon, exitIcon, startIcon, joinRandomIcon, generateCodeIcon, joinByCodeIcon;
    Shooter particleEffects;
    SDL_Renderer *renderer;
    void pressButtons(int x, int y, Client &client);

public:
    Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer);
    bool run(Client &client);
};
Menu::Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer) : SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), cancelIcon(renderer, "cancel", 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), exitIcon(renderer, "exit", 0.9 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), startIcon(renderer, "start", 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), joinRandomIcon(renderer, "random", 0.5 * SCREEN_WIDTH, 0.7 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), generateCodeIcon(renderer, "create team", 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.3 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), joinByCodeIcon(renderer, "join team", 0.5 * SCREEN_WIDTH, 0.3 * SCREEN_HEIGHT, 0.3 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), particleEffects(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 10, Game::shooterImages), renderer(renderer)
{
}
void Menu::pressButtons(int x, int y, Client &client)
{
    if (this->startIcon.isPressed(x, y))
    {
        this->joinRandomIcon.enable();
        this->joinByCodeIcon.enable();
        this->generateCodeIcon.enable();
        this->startIcon.disable();
    }
    else if (this->joinRandomIcon.isPressed(x, y))
        client.joinRandom();
    else
    {
        this->joinRandomIcon.disable();
        this->joinByCodeIcon.disable();
        this->generateCodeIcon.disable();
        this->startIcon.enable();
    }
}
bool Menu::run(Client &client)
{
    SDL_SetRelativeMouseMode(SDL_FALSE);
    while (true)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return true;
            case SDL_MOUSEBUTTONDOWN:
                if (this->exitIcon.isPressed(event.motion.x, event.motion.y))
                    return true;
                this->particleEffects.sprinkleFire(event.motion.x, event.motion.y, 30, 30, 0, 0);
                this->pressButtons(event.motion.x, event.motion.y, client);
                break;
            case SDL_MOUSEMOTION:
                uint32_t distance = sqrt(event.motion.xrel * event.motion.xrel + event.motion.yrel * event.motion.yrel);
                this->particleEffects.sprinkleSnow(event.motion.x, event.motion.y, distance, 0.5f * distance, 90, 0.1f * distance);
                break;
            }
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, Game::backgroundImage, NULL, NULL);
        this->cancelIcon.render(renderer);
        this->exitIcon.render(renderer);
        this->startIcon.render(renderer);
        this->generateCodeIcon.render(renderer);
        this->joinByCodeIcon.render(renderer);
        this->joinRandomIcon.render(renderer);
        this->particleEffects.sprinkler.render(renderer, this->particleEffects.time);
        SDL_RenderPresent(renderer);
        FPS_manager(Game::FRAME_GAP);
        this->particleEffects.time++;
        if (client.getId())
        {
            if (client.getMatch().active)
                break;
        }
    }
    return false;
}
