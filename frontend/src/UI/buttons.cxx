#pragma once
#include <stdexcept>
#include <SDL2/SDL_ttf.h>

class Icon
{
    bool imageOwner = false, isTemporaryInstance = false;
    SDL_Texture *image = NULL;

protected:
    SDL_Rect rect;

public:
    Icon(SDL_Texture *image, int x, int y, int w, int h);
    Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y, int w, bool isTemporaryInstance = false);
    Icon(const Icon &other);
    Icon &operator=(const Icon &other);
    ~Icon();
    inline float getAspectRatio();
    inline void setRect(int x, int y, int w, int h);
    inline void setRectPosition(int x, int y);
    inline void setRectDimention(int w, int h);
    inline SDL_Rect getRect();
    inline void change(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color);
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
Icon::Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y, int w, bool isTemporaryInstance) : imageOwner(true), isTemporaryInstance(isTemporaryInstance)
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
    w *= text.length();
    this->rect = createRect(x, y, w, w * float(surface->h) / surface->w);
    SDL_FreeSurface(surface);
}
Icon::Icon(const Icon &other) : imageOwner(other.imageOwner and other.isTemporaryInstance), image(other.image), rect(other.rect)
{
    if (other.imageOwner and not other.isTemporaryInstance)
    {
        std::cerr << "ERROR: Not allowed to invoke copy constructor with Icon constructed by: Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y, bool isTemporaryInstance = false)\nNote: Set isTemporaryInstance = true to make a temporary Instance which shouldn't be used for any perpose except for invoking copy constructor or = operator";
        exit(1);
    }
}
Icon &Icon::operator=(const Icon &other)
{
    if (this->imageOwner and not this->isTemporaryInstance)
        SDL_DestroyTexture(this->image);
    if (other.imageOwner and not other.isTemporaryInstance)
    {
        std::cerr << "ERROR: Not allowed to assign a Icon constructed by: Icon(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color, int x, int y, bool isTemporaryInstance = false)\nNote: Set isTemporaryInstance = true to make a temporary Instance which shouldn't be used for any perpose except for invoking = operator or copy constructor";
        exit(1);
    }
    else
    {
        this->imageOwner = other.imageOwner;
        this->image = other.image;
        this->rect = other.rect;
    }
    return *this;
}
Icon::~Icon()
{
    if (this->imageOwner and not this->isTemporaryInstance)
        SDL_DestroyTexture(this->image);
}
float Icon::getAspectRatio()
{
    int width, height;
    SDL_QueryTexture(image, NULL, NULL, &width, &height);
    return float(height) / width;
}
void Icon::setRect(int x, int y, int w, int h)
{
    this->rect = createRect(x, y, w, h);
}
void Icon::setRectPosition(int x, int y)
{
    this->rect = createRect(x, y, this->rect.w, this->rect.h);
}
void Icon::setRectDimention(int w, int h)
{
    this->rect = createRect(this->rect.x + this->rect.w / 2, this->rect.y + this->rect.h / 2, w, h);
}
SDL_Rect Icon::getRect()
{
    return this->rect;
}
void Icon::change(SDL_Renderer *renderer, const std::string &text, TTF_Font *font, SDL_Color color)
{
    if (this->imageOwner)
        SDL_DestroyTexture(this->image);
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
    this->rect = createRect(this->rect.x, this->rect.y, surface->w, surface->h);
    SDL_FreeSurface(surface);
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