class Game
{
    friend class Menu;

public:
    //**** ADJUSTABLE CONSTANT PARAMETERS ****
    static constexpr int FPS = 30;
    static constexpr int FRAME_GAP = 1000 / FPS;
    const int SCREEN_WIDTH, SCREEN_HEIGHT;

    static std::unordered_map<std::string, SDL_Texture *> buttonImages, shooterImages, deathImages;
    static SDL_Texture *backgroundImage;
    static TTF_Font *font;

private:
    SDL_Renderer *renderer;
    Shooter player1, player2;
    InputManager inputManager;
    Client &client;
    unsigned int gameId;

public:
    Game(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer, Client &client);
    static void loadResources(SDL_Renderer *renderer, std::string path);
    bool run();
};
std::unordered_map<std::string, SDL_Texture *> Game::buttonImages, Game::shooterImages, Game::deathImages;
SDL_Texture *Game::backgroundImage;
TTF_Font *Game::font;
Game::Game(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer, Client &client) : renderer(renderer), SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), inputManager(player1, buttonImages, SCREEN_WIDTH, SCREEN_HEIGHT), client(client)
{
    this->gameId = this->client.getId();
    float NormalPositionY1 = 0.2, NormalPositionY2 = 0.8;
    if (this->gameId % 10 == 1)
        std::swap(NormalPositionY1, NormalPositionY2);

    this->player1 = Shooter(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH / 2, SCREEN_HEIGHT * NormalPositionY1, 10, Game::shooterImages);
    this->player2 = Shooter(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH / 2, SCREEN_HEIGHT * NormalPositionY2, 10, Game::shooterImages);
}
void Game::loadResources(SDL_Renderer *renderer, std::string path)
{
    shooterImages["shooter"] = IMG_LoadTexture(renderer, (path + "/shooter.png").c_str());
    shooterImages["gun"] = IMG_LoadTexture(renderer, (path + "/wizard.png").c_str());
    shooterImages["snowBall"] = IMG_LoadTexture(renderer, (path + "/snowBall.png").c_str());
    shooterImages["fireBall"] = IMG_LoadTexture(renderer, (path + "/fireBall.png").c_str());
    shooterImages["web"] = IMG_LoadTexture(renderer, (path + "/web.png").c_str());
    shooterImages["snowParticle_1"] = IMG_LoadTexture(renderer, (path + "/snowParticle_1.png").c_str());
    shooterImages["snowParticle_2"] = IMG_LoadTexture(renderer, (path + "/snowParticle_2.png").c_str());
    shooterImages["fireParticle_1"] = IMG_LoadTexture(renderer, (path + "/fireParticle_1.png").c_str());
    shooterImages["fireParticle_2"] = IMG_LoadTexture(renderer, (path + "/fireParticle_2.png").c_str());
    shooterImages["magic_1"] = IMG_LoadTexture(renderer, (path + "/magic_1.png").c_str());
    shooterImages["magic_2"] = IMG_LoadTexture(renderer, (path + "/magic_2.png").c_str());
    shooterImages["magic_3"] = IMG_LoadTexture(renderer, (path + "/magic_3.png").c_str());
    shooterImages["snowBurst"] = IMG_LoadTexture(renderer, (path + "/snowBurst.png").c_str());
    shooterImages["fireBurst"] = IMG_LoadTexture(renderer, (path + "/fireBurst.png").c_str());
    shooterImages["spark_1"] = IMG_LoadTexture(renderer, (path + "/spark_1.png").c_str());
    shooterImages["spark_2"] = IMG_LoadTexture(renderer, (path + "/spark_2.png").c_str());
    shooterImages["frost"] = IMG_LoadTexture(renderer, (path + "/frost.png").c_str());
    deathImages["gunDeath_1"] = IMG_LoadTexture(renderer, (path + "/wizardDeath_1.png").c_str());
    deathImages["gunDeath_2"] = IMG_LoadTexture(renderer, (path + "/wizardDeath_2.png").c_str());
    deathImages["gunDeath_3"] = IMG_LoadTexture(renderer, (path + "/wizardDeath_3.png").c_str());
    buttonImages["snowBall"] = IMG_LoadTexture(renderer, (path + "/snowBall.png").c_str());
    buttonImages["fireBall"] = IMG_LoadTexture(renderer, (path + "/fireBall.png").c_str());
    buttonImages["web"] = IMG_LoadTexture(renderer, (path + "/web.png").c_str());
    buttonImages["joystickPad"] = IMG_LoadTexture(renderer, (path + "/joystickPad.png").c_str());
    buttonImages["joystickButton"] = IMG_LoadTexture(renderer, (path + "/joystickButton.png").c_str());
    buttonImages["button"] = IMG_LoadTexture(renderer, (path + "/button.png").c_str());
    backgroundImage = IMG_LoadTexture(renderer, (path + "/background.png").c_str());
    font = TTF_OpenFont((path + "/bombing.ttf").c_str(), 24);
}
bool Game::run()
{
    while (not this->client.initMatch(this->player1, this->player2))
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                return true;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                case SDLK_ESCAPE:
                    return false;
                }
            }
        }
        SDL_RenderClear(this->renderer);
        SDL_RenderCopy(this->renderer, backgroundImage, NULL, NULL);
        SDL_RenderPresent(this->renderer);
        FPS_manager(FRAME_GAP);
    }
    SDL_SetRelativeMouseMode(SDL_TRUE);
    while (this->inputManager.handelInput())
    {
        this->player1.update();
        this->player2.update();
        collisionHandeler(this->player1, this->player2);
        this->player1.render(this->renderer);
        this->player2.render(this->renderer);
        this->inputManager.render(this->renderer);
        SDL_RenderPresent(this->renderer);
        SDL_RenderClear(this->renderer);
        FPS_manager(FRAME_GAP);
        if (!client.sendAndRecieve(player1, player2))
            return false;
    }
    return true;
}