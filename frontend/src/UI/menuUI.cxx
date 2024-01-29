#include "buttons.cxx"
class DialogueBox : public Icon
{
protected:
    std::vector<Icon> text;
    uint32_t width, height;
    int paragraphHeight;
    bool active;
    uint32_t targetWidth, targetHeight;
    int targetParagraphHeight;
    void format();

public:
    DialogueBox(SDL_Renderer *renderer, SDL_Texture *images, int x, int y, int w, int h, bool active = true, float margin_normal = 0.1);
    DialogueBox(SDL_Renderer *renderer, std::string line, SDL_Texture *image, int x, int y, int w, int h, bool active = true, float margin_normal = 0.1);
    inline bool isActive();
    inline void disable();
    inline void enable();
    inline void changeText(SDL_Renderer *renderer, std::vector<std::string> &text, TTF_Font *font, SDL_Color color);
    void render(SDL_Renderer *renderer, uint8_t alpha = 255);
    float dimentionPercent();
};
void DialogueBox::format()
{
    for (uint32_t lineNo = 0; lineNo < this->text.size(); lineNo++)
    {
        text[lineNo].setRectPosition(this->rect.x + this->rect.w / 2, this->rect.y + (lineNo + 1) * this->rect.h / (this->text.size() + 1));
    }
}
DialogueBox::DialogueBox(SDL_Renderer *renderer, SDL_Texture *image, int x, int y, int w, int h, bool active, float margin_normal) : Icon(image, x, y, 0, 0), width(w), height(h), paragraphHeight((1 - margin_normal) * h)
{
    int boxWidth, boxHeight;
    SDL_QueryTexture(image, NULL, NULL, &boxWidth, &boxHeight);
    if (active)
        this->enable();
    else
        this->disable();
}
DialogueBox::DialogueBox(SDL_Renderer *renderer, std::string line, SDL_Texture *image, int x, int y, int w, int h, bool active, float margin_normal) : Icon(image, x, y, 0, 0), text(1), width(w), height(h), paragraphHeight((1 - margin_normal) * h)
{
    int boxWidth, boxHeight;
    SDL_QueryTexture(image, NULL, NULL, &boxWidth, &boxHeight);
    this->text[0] = Icon(renderer, line, Game::font, {255, 255, 0, 255}, 0, 0, 0, true);
    this->format();
    if (active)
        this->enable();
    else
        this->disable();
}
bool DialogueBox::isActive()
{
    return this->active;
}
void DialogueBox::disable()
{
    this->targetHeight = 0;
    this->targetWidth = 0;
    this->targetParagraphHeight = 0;
    this->active = false;
}
void DialogueBox::enable()
{
    this->targetHeight = this->height;
    this->targetWidth = this->width;
    this->targetParagraphHeight = this->paragraphHeight;
    this->active = true;
}
void DialogueBox::changeText(SDL_Renderer *renderer, std::vector<std::string> &text, TTF_Font *font, SDL_Color color)
{
    if (this->text.size() != text.size())
        this->text = std::vector<Icon>(text.size());
    for (int line_no = 0; line_no < this->text.size(); line_no++)
        this->text[line_no].change(renderer, text[line_no], font, color);
    this->format();
}
void DialogueBox::render(SDL_Renderer *renderer, uint8_t alpha)
{
    if (not(this->active or this->rect.w))
        return;
    this->setRectDimention((this->rect.w + this->targetWidth) / 2, (this->rect.h + this->targetHeight) / 2);
    this->format();
    Icon::render(renderer, alpha);
    for (Icon &line : this->text)
    {
        int letterHeight = this->targetParagraphHeight / text.size();
        line.setRectDimention((letterHeight / line.getAspectRatio() + line.getRect().w) / 2, (letterHeight + line.getRect().h) / 2);
        line.render(renderer, alpha);
    }
}
float DialogueBox::dimentionPercent()
{
    return float(this->rect.w) / this->width;
}

class ButtonIcon : public DialogueBox
{
public:
    ButtonIcon(SDL_Renderer *renderer, std::string name, int x, int y, int w, int h, bool active = true, float margin_normal = 0.2f);
    inline bool isPressed(int x, int y);
};
ButtonIcon::ButtonIcon(SDL_Renderer *renderer, std::string name, int x, int y, int w, int h, bool active, float margin_normal) : DialogueBox(renderer, name, Game::buttonImages["button"], x, y, w, h, active, margin_normal)
{
}
bool ButtonIcon::isPressed(int x, int y)
{
    return active and x - rect.x < rect.w and x - rect.x > 0 and y - rect.y < rect.h and y - rect.y > 0;
}

class Menu
{
public:
    enum class Result
    {
        CANCELLED,
        PLAY,
        EXIT
    };

private:
    const int SCREEN_WIDTH, SCREEN_HEIGHT;
    ButtonIcon cancelIcon, exitIcon, startIcon, joinRandomIcon, generateCodeIcon, joinByCodeIcon;
    DialogueBox dialogueBox;
    Shooter particleEffects;
    SDL_Renderer *renderer;
    std::string code;
    Menu::Result pressButtons(int x, int y, Client &client);

public:
    Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer);
    Menu::Result run(Client &client);
};
Menu::Menu(int SCREEN_WIDTH, int SCREEN_HEIGHT, SDL_Renderer *renderer) : SCREEN_WIDTH(SCREEN_WIDTH), SCREEN_HEIGHT(SCREEN_HEIGHT), cancelIcon(renderer, "cancel", 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.3 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), exitIcon(renderer, "exit", 0.9 * SCREEN_WIDTH, 0.1 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), startIcon(renderer, "start", 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.2 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT), joinRandomIcon(renderer, "random", 0.5 * SCREEN_WIDTH, 0.7 * SCREEN_HEIGHT, 0.25 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), generateCodeIcon(renderer, "create team", 0.5 * SCREEN_WIDTH, 0.5 * SCREEN_HEIGHT, 0.35 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), joinByCodeIcon(renderer, "join team", 0.5 * SCREEN_WIDTH, 0.3 * SCREEN_HEIGHT, 0.35 * SCREEN_WIDTH, 0.2 * SCREEN_HEIGHT, false), dialogueBox(renderer, Game::buttonImages["box"], SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, 2 * SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2, false), particleEffects(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0, 10, Game::shooterImages), renderer(renderer)
{
}
Menu::Result Menu::pressButtons(int x, int y, Client &client)
{
    if (this->startIcon.isPressed(x, y))
    {
        this->joinRandomIcon.enable();
        this->joinByCodeIcon.enable();
        this->generateCodeIcon.enable();
        this->startIcon.disable();
    }
    else if (this->joinRandomIcon.isPressed(x, y))
    {
        std::vector<std::string> text;
        SDL_Colour color;
        if (client.joinRandom())
        {
            color = {0, 0, 0, 255};
            text = {std::string("On queue"), std::string("searching...")};
        }
        else
        {
            color = {150, 0, 0, 255};
            text = {std::string("Server unreachable!"), std::string("check your internet"), std::string("Try again")};
        }
        this->dialogueBox.changeText(this->renderer, text, Game::font, color);
        this->dialogueBox.enable();
        goto matchMaking;
    }
    else if (this->generateCodeIcon.isPressed(x, y))
    {
        std::vector<std::string> text;
        SDL_Colour color;
        if (client.generateCode())
        {
            color = {0, 0, 0, 255};
            text = {std::string("Team code:"), std::to_string(client.getId())};
        }
        else
        {
            color = {150, 0, 0, 255};
            text = {std::string("Server unreachable!"), std::string("check your internet"), std::string("Try again")};
        }
        this->dialogueBox.changeText(this->renderer, text, Game::font, color);
        this->dialogueBox.enable();
        goto matchMaking;
    }
    else if (this->joinByCodeIcon.isPressed(x, y))
    {
        std::vector<std::string> text;
        if (this->code.length())
            text = {std::string("Enter code:"), this->code};
        else
            text = {std::string("Enter code:"), std::string("_ _ _ _ _ _")};
        this->dialogueBox.changeText(this->renderer, text, Game::font, {0, 0, 0, 255});
        this->dialogueBox.enable();
    }
    else if (this->cancelIcon.isPressed(x, y))
        return Menu::Result::CANCELLED;
    else if (this->exitIcon.isPressed(x, y))
        return Menu::Result::EXIT;
    else if (not this->cancelIcon.isActive())
    {
        if (this->dialogueBox.isActive())
            this->dialogueBox.disable();
        else
        {
            this->joinRandomIcon.disable();
            this->joinByCodeIcon.disable();
            this->generateCodeIcon.disable();
            this->startIcon.enable();
        }
    }
    return Menu::Result::PLAY;
matchMaking:
    this->joinRandomIcon.disable();
    this->joinByCodeIcon.disable();
    this->generateCodeIcon.disable();
    this->startIcon.disable();
    this->cancelIcon.enable();
    return Menu::Result::PLAY;
}
Menu::Result Menu::run(Client &client)
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
                return Menu::Result::EXIT;
            case SDL_MOUSEBUTTONDOWN:
            {
                this->particleEffects.sprinkleFire(event.motion.x, event.motion.y, 30, 30, 0, 0);
                Menu::Result result = this->pressButtons(event.motion.x, event.motion.y, client);
                if (result != Menu::Result::PLAY)
                    return result;
            }
            break;
            case SDL_KEYDOWN:
                if (not this->dialogueBox.isActive())
                    break;
                switch (event.key.keysym.sym)
                {
                case SDLK_0:
                    this->code.append("0");
                    break;
                case SDLK_1:
                    this->code.append("1");
                    break;
                case SDLK_2:
                    this->code.append("2");
                    break;
                case SDLK_3:
                    this->code.append("3");
                    break;
                case SDLK_4:
                    this->code.append("4");
                    break;
                case SDLK_5:
                    this->code.append("5");
                    break;
                case SDLK_6:
                    this->code.append("6");
                    break;
                case SDLK_7:
                    this->code.append("7");
                    break;
                case SDLK_8:
                    this->code.append("8");
                    break;
                case SDLK_9:
                    this->code.append("9");
                    break;
                case SDLK_BACKSPACE:
                    this->code.pop_back();
                    break;
                case SDLK_RETURN:
                    if (not client.joinByCode(this->code))
                    {
                        std::vector<std::string> text = {std::string("Server unreachable!"), std::string("check your internet"), std::string("Try again")};
                        this->dialogueBox.changeText(this->renderer, text, Game::font, {150, 0, 0, 255});
                        this->dialogueBox.enable();
                    }
                    else if (not client.getId())
                    {
                        std::vector<std::string> text = {std::string("Wrong code!"), std::string("No game has code:"), this->code};
                        this->dialogueBox.changeText(this->renderer, text, Game::font, {150, 0, 0, 255});
                        this->dialogueBox.enable();
                    }
                    goto end;
                }
                {
                    std::vector<std::string> text = {std::string("Enter code:"), this->code};
                    this->dialogueBox.changeText(this->renderer, text, Game::font, {0, 0, 100, 255});
                }
            end:
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
        this->dialogueBox.render(renderer, this->dialogueBox.dimentionPercent() * 200);
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
    return Menu::Result::PLAY;
}
