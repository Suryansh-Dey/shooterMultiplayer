class BallSelectButtons : public SelectableOptions
{
    Shooter *player;

public:
    void dummy() {}
    BallSelectButtons(Shooter *player, std::unordered_map<std::string, SDL_Texture *> images, int SCREEN_WIDTH, int SCREEN_HEIGHT);
    void shoot();
};

class InputManager
{
    int SCREEN_WIDTH, SCREEN_HEIGHT;
    float MOUSE_SENSITIVITY = 1;
    Shooter &player;
    SDL_Texture *image;

    BallSelectButtons ballSelectButtons;
    Joystick joystick;

    inline void pressButtons(SDL_Event event);
    inline void unpressButtons(SDL_Event event);

public:
    InputManager(Shooter &player, std::unordered_map<std::string, SDL_Texture *> images, int SCREEN_WIDTH, int SCREEN_HEIGHT);
    bool handelInput();
    void render(SDL_Renderer *s);
};

BallSelectButtons::BallSelectButtons(Shooter *player, std::unordered_map<std::string, SDL_Texture *> images, int SCREEN_WIDTH, int SCREEN_HEIGHT) : SelectableOptions(std::unordered_map<std::string, Icon>{
                                                                                                                                                        {"snowBall", Icon(images["snowBall"], 0.4 * SCREEN_WIDTH, 0.9 * SCREEN_HEIGHT, 0.1 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT)},
                                                                                                                                                        {"fireBall", Icon(images["fireBall"], 0.5 * SCREEN_WIDTH, 0.9 * SCREEN_HEIGHT, 0.1 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT)},
                                                                                                                                                        {"web", Icon(images["web"], 0.6 * SCREEN_WIDTH, 0.9 * SCREEN_HEIGHT, 0.1 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT)}}),
                                                                                                                                                    player(player)
{
}
void BallSelectButtons::shoot()
{
    if (this->selectedOption == "snowBall")
        this->player->shoot(Shooter::bulletType::snowBall);
    else if (this->selectedOption == "fireBall")
        this->player->shoot(Shooter::bulletType::fireBall);
    else if (this->selectedOption == "web")
        this->player->shoot(Shooter::bulletType::web);
}

InputManager::InputManager(Shooter &player, std::unordered_map<std::string, SDL_Texture *> images, int SCREEN_WIDTH, int SCREEN_HEIGHT) : player(player), SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), ballSelectButtons(&player, images, SCREEN_WIDTH, SCREEN_HEIGHT)
{
}
bool InputManager::handelInput()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_QUIT:
            return false;
            break;
        case SDL_KEYDOWN:
            this->pressButtons(event);
            break;
        case SDL_KEYUP:
            this->unpressButtons(event);
            break;
        case SDL_MOUSEBUTTONDOWN:
            this->ballSelectButtons.shoot();
            break;
        case SDL_MOUSEMOTION:
            this->player.rotateGun(angleOfVector(event.motion.x - this->player.get_position().x, event.motion.y - this->player.get_position().y)-player.get_gunAngle_degree());
        }
    }
    if (this->joystick.isPressed())
        this->player.move(this->joystick.getAngle_degree());
    return true;
}
void InputManager::render(SDL_Renderer *renderer)
{
    this->ballSelectButtons.render(renderer);
}
void InputManager::pressButtons(SDL_Event event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_0:
        this->ballSelectButtons.select("NULL");
        break;
    case SDLK_1:
        this->ballSelectButtons.select("snowBall");
        break;
    case SDLK_2:
        this->ballSelectButtons.select("fireBall");
        break;
    case SDLK_3:
        this->ballSelectButtons.select("web");
        break;
    case SDLK_w:
        this->joystick.pressUp();
        break;
    case SDLK_s:
        this->joystick.pressDown();
        break;
    case SDLK_a:
        this->joystick.pressLeft();
        break;
    case SDLK_d:
        this->joystick.pressRight();
        break;
    case SDLK_RCTRL:
        if (SDL_GetRelativeMouseMode() == SDL_TRUE)
            SDL_SetRelativeMouseMode(SDL_FALSE);
        else
            SDL_SetRelativeMouseMode(SDL_TRUE);
        break;
    }
}
void InputManager::unpressButtons(SDL_Event event)
{
    switch (event.key.keysym.sym)
    {
    case SDLK_w:
        this->joystick.unpressUp();
        break;
    case SDLK_s:
        this->joystick.unpressDown();
        break;
    case SDLK_a:
        this->joystick.unpressLeft();
        break;
    case SDLK_d:
        this->joystick.unpressRight();
        break;
    }
}