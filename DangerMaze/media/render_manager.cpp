#include "render_manager.h"

using namespace std;
using namespace media;

RenderManager::RenderManager() noexcept
    : _renderer(nullptr)
    , _color({ 255, 255, 255, 255 })
{
}

RenderManager::RenderManager(SDL_Window* window) noexcept
    : _color({255, 255, 255, 255})
{
    _renderer = SDL_CreateRenderer(
        window,
        -1,                         // выбрать драйвер для рендеринга автоматически
        SDL_RENDERER_ACCELERATED |  // аппаратное ускорение
        SDL_RENDERER_PRESENTVSYNC   // вертикальная синхронизация
    );
}

RenderManager::~RenderManager() {
    SDL_DestroyRenderer(_renderer);
}

RenderManager::RenderManager(RenderManager&& otherManager) noexcept
    : _renderer(otherManager._renderer)
    , _priorityToSprites(otherManager._priorityToSprites)
    , _color(otherManager._color)
{
    otherManager._renderer = nullptr;
}

void RenderManager::operator=(RenderManager&& otherManager) {
    _renderer = otherManager._renderer;
    _priorityToSprites = otherManager._priorityToSprites;
    _color = otherManager._color;
    
    otherManager._renderer = nullptr;
}

void RenderManager::setColor(const SDL_Color& color) {
    _color = color;
}

SDL_Renderer* RenderManager::getRenderer() noexcept {
    return _renderer;
}

void RenderManager::addToQueue(SpritePtr sprite, const SDL_Point& point, int priority) {
    _priorityToSprites[priority].push_back({ point, sprite });
}

void RenderManager::clearQueue() {
    for (auto& i : _priorityToSprites) {
        i.second.clear();
    }
}

void RenderManager::renderAll(const object::CameraPtr camera) const {
    SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, _color.a);
    SDL_RenderClear(_renderer);

    SDL_Point cameraPoint = camera->getPosition();

    for (const auto& i : _priorityToSprites) {
        for (const auto& [point, sprite]  : i.second) {
            SDL_Point renderPoint = point;
            renderPoint.x -= cameraPoint.x;
            renderPoint.y -= cameraPoint.y;
            sprite->copyTo(_renderer, renderPoint);
        }
    }

    SDL_RenderPresent(_renderer);
}
