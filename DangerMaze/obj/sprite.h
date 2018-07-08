#pragma once

#include <string>
#include <memory>

#include "SDL.h"

namespace obj {

    class Sprite;

    using SpritePtr = std::shared_ptr<Sprite>;

    class Sprite {
    public:
        static obj::SpritePtr       createFromPng(const std::string& path, SDL_Renderer* renderer);

        Sprite() noexcept;
        Sprite(SDL_Texture* texture) noexcept;
        ~Sprite();
        
        SDL_Texture*                getTexture() const noexcept;
        int                         getWidth() const noexcept;
        int                         getHeight() const noexcept;

    private:
        SDL_Texture*                _texture;
        int                         _width;
        int                         _height;
    };

}
