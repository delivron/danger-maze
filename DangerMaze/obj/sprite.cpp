#include <iostream>
#include <stdexcept>

#include "SDL_image.h"

#include "sprite.h"

using namespace std;
using namespace obj;

SpritePtr Sprite::createFromPng(const std::string& path, SDL_Renderer* renderer) {
    SDL_Surface* pngImage = IMG_Load(path.c_str());
    if (pngImage == nullptr) {
        throw runtime_error("Error [IMG_Load]: " + path);
    }

    SpritePtr sprite = make_shared<Sprite>(
        SDL_CreateTextureFromSurface(renderer, pngImage)
    );
    SDL_FreeSurface(pngImage);

    if (sprite->getTexture() == nullptr) {
        throw runtime_error("Error [SDL_CreateTextureFromSurface]: " + path);
    }

    return sprite;
}

Sprite::Sprite() noexcept
    : _texture(nullptr)
    , _width(0)
    , _height(0)
{
}

Sprite::Sprite(SDL_Texture* texture) noexcept
    : _texture(texture)
{
    SDL_QueryTexture(_texture, nullptr, nullptr, &_width, &_height);
}

Sprite::~Sprite() {
    SDL_DestroyTexture(_texture);
}

SDL_Texture* Sprite::getTexture() const noexcept {
    return _texture;
}

int Sprite::getWidth() const noexcept {
    return _width;
}

int Sprite::getHeight() const noexcept {
    return _height;
}
