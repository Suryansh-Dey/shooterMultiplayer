class Game
{
    //**** ADJUSTABLE CONSTANT PARAMETERS ****
    const int FPS = 30;
    const int FRAME_GAP = 1000 / FPS;
    const int SCREEN_WIDTH, SCREEN_HEIGHT;

    SDL_Renderer *renderer;
    static std::unordered_map<std::string, SDL_Texture *> buttonImages, shooterImages, deathImages;
    Shooter player1, player2;
    InputManager inputManager;
    Client client;
    unsigned int gameId;

public:
    Game(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer);
    static void loadResources(SDL_Renderer *renderer);
    bool main();
};
std::unordered_map<std::string, SDL_Texture *> Game::buttonImages, Game::shooterImages, Game::deathImages;
Game::Game(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer) : renderer(renderer), SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), inputManager(NULL, buttonImages, SCREEN_WIDTH, SCREEN_HEIGHT), client("http://localhost:3000", SCREEN_WIDTH, SCREEN_HEIGHT)
{
    this->client.joinRandom();
    this->gameId = this->client.getId();
    float NormalPositionY1 = 0.2, NormalPositionY2 = 0.8;
    if (this->gameId % 10 == 1)
        std::swap(NormalPositionY1, NormalPositionY2);

    this->player1 = Shooter(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH / 2, SCREEN_HEIGHT * NormalPositionY1, 10, Game::shooterImages);
    this->player2 = Shooter(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_WIDTH / 2, SCREEN_HEIGHT * NormalPositionY2, 10, Game::shooterImages);
    this->inputManager = InputManager(&(this->player1), this->buttonImages, SCREEN_WIDTH, SCREEN_HEIGHT);
}
void Game::loadResources(SDL_Renderer *renderer)
{
    shooterImages["shooter"] = IMG_LoadTexture(renderer, "resources/shooter.png");
    shooterImages["gun"] = IMG_LoadTexture(renderer, "resources/wizard.png");
    shooterImages["snowBall"] = IMG_LoadTexture(renderer, "resources/snowBall.png");
    shooterImages["fireBall"] = IMG_LoadTexture(renderer, "resources/fireBall.png");
    shooterImages["web"] = IMG_LoadTexture(renderer, "resources/web.png");
    shooterImages["snowParticle_1"] = IMG_LoadTexture(renderer, "resources/snowParticle_1.png");
    shooterImages["snowParticle_2"] = IMG_LoadTexture(renderer, "resources/snowParticle_2.png");
    shooterImages["fireParticle_1"] = IMG_LoadTexture(renderer, "resources/fireParticle_1.png");
    shooterImages["fireParticle_2"] = IMG_LoadTexture(renderer, "resources/fireParticle_2.png");
    shooterImages["magic_1"] = IMG_LoadTexture(renderer, "resources/magic_1.png");
    shooterImages["magic_2"] = IMG_LoadTexture(renderer, "resources/magic_2.png");
    shooterImages["magic_3"] = IMG_LoadTexture(renderer, "resources/magic_3.png");
    shooterImages["snowBurst"] = IMG_LoadTexture(renderer, "resources/snowBurst.png");
    shooterImages["fireBurst"] = IMG_LoadTexture(renderer, "resources/fireBurst.png");
    shooterImages["spark_1"] = IMG_LoadTexture(renderer, "resources/spark_1.png");
    shooterImages["spark_2"] = IMG_LoadTexture(renderer, "resources/spark_2.png");
    shooterImages["frost"] = IMG_LoadTexture(renderer, "resources/frost.png");
    deathImages["gunDeath_1"] = IMG_LoadTexture(renderer, "resources/wizardDeath_1.png");
    deathImages["gunDeath_2"] = IMG_LoadTexture(renderer, "resources/wizardDeath_2.png");
    deathImages["gunDeath_3"] = IMG_LoadTexture(renderer, "resources/wizardDeath_3.png");
    buttonImages["snowBall"] = IMG_LoadTexture(renderer, "resources/snowBall.png");
    buttonImages["fireBall"] = IMG_LoadTexture(renderer, "resources/fireBall.png");
    buttonImages["web"] = IMG_LoadTexture(renderer, "resources/web.png");
    buttonImages["joystickPad"] = IMG_LoadTexture(renderer, "resources/joystickPad.png");
    buttonImages["joystickButton"] = IMG_LoadTexture(renderer, "resources/joystickButton.png");
    buttonImages["train"] = IMG_LoadTexture(renderer, "resources/train.png");
    buttonImages["select"] = IMG_LoadTexture(renderer, "resources/select.png");
}
bool Game::main()
{
    while (!this->client.getMatch().active)
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
        client.sendAndRecieve(player1, player2);
    }
    return true;
}