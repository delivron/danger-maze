#pragma once

#include <map>
#include <vector>
#include <utility>

#include "SDL.h"

#include "sprite.h"

#include "../object/camera.h"

namespace media {

    class RenderManager {
    public:
        RenderManager() noexcept;
        explicit RenderManager(SDL_Window* window) noexcept;
        ~RenderManager();
        RenderManager(RenderManager&& otherManager) noexcept;

        void                        operator=(RenderManager&& otherManager);

        void                        setColor(const SDL_Color& color);
        SDL_Renderer*               getRenderer() noexcept;
        void                        addToQueue(SpritePtr sprite, const SDL_Point& point, int priority = 0);
        void                        clearQueue();
        void                        renderAll(const object::CameraPtr camera) const;

    private:
        using Sprites               = std::vector<std::pair<SDL_Point, SpritePtr>>;
        
        SDL_Renderer*               _renderer;
        SDL_Color                   _color;
        std::map<int, Sprites>      _priorityToSprites;
    };

}
