#pragma once
#include <stdexcept>
#include <SDL2/SDL_ttf.h>

class Icon
{
    bool imageOwner = false;

protected:
    SDL_Texture *image = NULL;
    SDL_Rect rect;

public:
    Icon(SDL_Texture *image, int x, int y, int w, int h);
    Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y);
    Icon(const Icon &other);
    Icon &operator=(const Icon &other);
    ~Icon();
    void render(SDL_Renderer *renderer, uint8_t intensity);
};

class Button : public Icon
{
protected:
    bool pressed = false;

public:
    virtual void dummy() = 0;
    Button(SDL_Texture *image, int x, int y, int w, int h);
    inline void unpress();
    inline bool isPressed();
};

class SelectableOptions
{
protected:
    std::unordered_map<std::string, Icon> icons;
    std::string selectedOption = "NULL";

public:
    virtual void dummy() = 0;
    SelectableOptions(std::unordered_map<std::string, Icon> icons);
    inline void select(std::string optionName);
    void render(SDL_Renderer *renderer);
};

class Joystick
{
    int8_t x = 0, y = 0;

public:
    inline void pressUp();
    inline void pressDown();
    inline void pressLeft();
    inline void pressRight();
    inline void unpressUp();
    inline void unpressDown();
    inline void unpressLeft();
    inline void unpressRight();
    inline bool isPressed();
    inline float getAngle_degree();
};

Icon::Icon(SDL_Texture *image, int x, int y, int w, int h) : image(image)
{
    this->rect = createRect(x, y, w, h);
}
Icon::Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y) : imageOwner(true)
{

    SDL_Surface *surface = TTF_RenderText_Solid(font, text.c_str(), color);
    if (surface == nullptr)
    {
        std::cerr << "Failed to create surface: " << SDL_GetError() << std::endl;
        return;
    }

    this->image = SDL_CreateTextureFromSurface(renderer, surface);
    if (this->image == nullptr)
    {
        std::cerr << "Failed to create texture: " << SDL_GetError() << std::endl;
        SDL_FreeSurface(surface);
        return;
    }
    this->rect = createRect(x, y, surface->w, surface->h);

    SDL_FreeSurface(surface);
}
Icon::Icon(const Icon &other) : imageOwner(false), image(other.image), rect(other.rect)
{
    if (this->imageOwner)
    {
        std::cerr << "Copy constructor of Icon constructed with: Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y) cannot be used\n";
        exit(1);
    }
}
Icon &Icon::operator=(const Icon &other)
{
    if (other.imageOwner)
    {
        std::cerr << "Copy constructor of Icon constructed with: Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y) cannot be used\n";
        exit(1);
    }
    else
    {
        this->image = other.image;
        this->rect = other.rect;
        this->imageOwner = false;
    }
    return *this;
}
Icon::~Icon()
{
    if (this->imageOwner)
        SDL_DestroyTexture(this->image);
}
void Icon::render(SDL_Renderer *renderer, uint8_t intensity = 255)
{
    SDL_SetTextureAlphaMod(this->image, intensity);
    SDL_RenderCopy(renderer, this->image, NULL, &(this->rect));
}

Button::Button(SDL_Texture *image, int x, int y, int w, int h) : Icon(image, x, y, w, h)
{
}
void Button::unpress()
{
    this->pressed = false;
}
bool Button::isPressed()
{
    return this->pressed;
}

SelectableOptions::SelectableOptions(std::unordered_map<std::string, Icon> icons) : icons(icons)
{
}
void SelectableOptions::select(std::string optionName)
{
    if (this->icons.find(optionName) == this->icons.end() and optionName != "NULL")
        throw std::runtime_error("Error: No option of name " + optionName + " was created but was passed to SelectableOptions::select(std::string)\n");
    this->selectedOption = optionName;
}
void SelectableOptions::render(SDL_Renderer *renderer)
{
    for (auto &icon : icons)
        icon.second.render(renderer, 100 + 155 * (icon.first == this->selectedOption));
}

void Joystick::pressUp()
{
    this->y = -1;
}
void Joystick::pressDown()
{
    this->y = 1;
}
void Joystick::pressLeft()
{
    this->x = -1;
}
void Joystick::pressRight()
{
    this->x = 1;
}
void Joystick::unpressUp()
{
    this->y += (-this->y) * (this->y == -1);
}
void Joystick::unpressDown()
{
    this->y += (-this->y) * (this->y == 1);
}
void Joystick::unpressLeft()
{
    this->x += (-this->x) * (this->x == -1);
}
void Joystick::unpressRight()
{
    this->x += (-this->x) * (this->x == 1);
}
bool Joystick::isPressed()
{
    return x or y;
}
float Joystick::getAngle_degree()
{
    return angleOfVector(this->x, this->y);
}