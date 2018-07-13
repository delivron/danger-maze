#pragma once

#include <queue>
#include <vector>
#include <utility>
#include <functional>

#include "SDL.h"

#include "sprite.h"

#include "../object/camera.h"

namespace media {

    struct RenderData {
        int                         priority;
        SDL_Point                   point;
        media::SpritePtr            sprite;
    };

    bool                            operator>(const RenderData& lhs, const RenderData& rhs);

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

        // Все объекты из очереди выводятся на экран, очередь очищается.
        void                        renderAll(const object::CameraPtr camera);

    private:
        using PriorityQueue         = std::priority_queue<RenderData, std::vector<RenderData>, std::greater<RenderData>>;

        SDL_Renderer*               _renderer;
        SDL_Color                   _color;
        PriorityQueue               _priorityQueue;
    };

}
