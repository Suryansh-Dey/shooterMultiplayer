class Game
{
    //**** ADJUSTABLE CONSTANT PARAMETERS ****
    const int FPS = 30;
    const int FRAME_GAP = 1000 / FPS;
    const int SCREEN_WIDTH, SCREEN_HEIGHT;

    SDL_Renderer *renderer;
    std::unordered_map<std::string, SDL_Texture *> buttonImages, shooterImages, deathImages;
    Shooter player1, player2;
    InputManager inputManager;
    Client client;

public:
    Game(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer);
    void loadResources();
    bool main();
};
Game::Game(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer) : renderer(renderer), SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), inputManager(NULL, buttonImages, SCREEN_WIDTH, SCREEN_HEIGHT), client("http://localhost:3000")
{
    loadResources();
    player1 = Shooter(SCREEN_WIDTH, SCREEN_HEIGHT, 100, 100, 10, shooterImages);
    player2 = Shooter(SCREEN_WIDTH, SCREEN_HEIGHT, 100, 400, 10, shooterImages);
    inputManager = InputManager(&player1, buttonImages, SCREEN_WIDTH, SCREEN_HEIGHT);
    SDL_SetRelativeMouseMode(SDL_TRUE);
}
void Game::loadResources()
{
    shooterImages["shooter"] = IMG_LoadTexture(this->renderer, "resources/shooter.png");
    shooterImages["gun"] = IMG_LoadTexture(this->renderer, "resources/wizard.png");
    shooterImages["snowBall"] = IMG_LoadTexture(this->renderer, "resources/snowBall.png");
    shooterImages["fireBall"] = IMG_LoadTexture(this->renderer, "resources/fireBall.png");
    shooterImages["web"] = IMG_LoadTexture(this->renderer, "resources/web.png");
    shooterImages["snowParticle_1"] = IMG_LoadTexture(this->renderer, "resources/snowParticle_1.png");
    shooterImages["snowParticle_2"] = IMG_LoadTexture(this->renderer, "resources/snowParticle_2.png");
    shooterImages["fireParticle_1"] = IMG_LoadTexture(this->renderer, "resources/fireParticle_1.png");
    shooterImages["fireParticle_2"] = IMG_LoadTexture(this->renderer, "resources/fireParticle_2.png");
    shooterImages["magic_1"] = IMG_LoadTexture(this->renderer, "resources/magic_1.png");
    shooterImages["magic_2"] = IMG_LoadTexture(this->renderer, "resources/magic_2.png");
    shooterImages["magic_3"] = IMG_LoadTexture(this->renderer, "resources/magic_3.png");
    shooterImages["snowBurst"] = IMG_LoadTexture(this->renderer, "resources/snowBurst.png");
    shooterImages["fireBurst"] = IMG_LoadTexture(this->renderer, "resources/fireBurst.png");
    shooterImages["spark_1"] = IMG_LoadTexture(this->renderer, "resources/spark_1.png");
    shooterImages["spark_2"] = IMG_LoadTexture(this->renderer, "resources/spark_2.png");
    shooterImages["frost"] = IMG_LoadTexture(this->renderer, "resources/frost.png");
    deathImages["gunDeath_1"] = IMG_LoadTexture(this->renderer, "resources/wizardDeath_1.png");
    deathImages["gunDeath_2"] = IMG_LoadTexture(this->renderer, "resources/wizardDeath_2.png");
    deathImages["gunDeath_3"] = IMG_LoadTexture(this->renderer, "resources/wizardDeath_3.png");
    buttonImages["snowBall"] = IMG_LoadTexture(this->renderer, "resources/snowBall.png");
    buttonImages["fireBall"] = IMG_LoadTexture(this->renderer, "resources/fireBall.png");
    buttonImages["web"] = IMG_LoadTexture(this->renderer, "resources/web.png");
    buttonImages["joystickPad"] = IMG_LoadTexture(this->renderer, "resources/joystickPad.png");
    buttonImages["joystickButton"] = IMG_LoadTexture(this->renderer, "resources/joystickButton.png");
    buttonImages["train"] = IMG_LoadTexture(this->renderer, "resources/train.png");
    buttonImages["select"] = IMG_LoadTexture(this->renderer, "resources/select.png");
}
bool Game::main()
{
    client.joinRandom();
    while (inputManager.handelInput())
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