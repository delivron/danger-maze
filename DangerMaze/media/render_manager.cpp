#include "render_manager.h"

using namespace std;
using namespace media;

bool media::operator>(const RenderData& lhs, const RenderData& rhs) {
    return lhs.priority > rhs.priority;
}

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
    , _priorityQueue(otherManager._priorityQueue)
    , _color(otherManager._color)
{
    otherManager._renderer = nullptr;
}

void RenderManager::operator=(RenderManager&& otherManager) {
    _renderer = otherManager._renderer;
    _priorityQueue = otherManager._priorityQueue;
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
    _priorityQueue.push(RenderData{ priority, point, sprite });
}

void RenderManager::renderAll(const object::CameraPtr camera) {
    SDL_SetRenderDrawColor(_renderer, _color.r, _color.g, _color.b, _color.a);
    SDL_RenderClear(_renderer);

    SDL_Point cameraPoint = camera->getPosition();

    while (!_priorityQueue.empty()) {
        const RenderData& top = _priorityQueue.top();
        SDL_Point renderPoint = top.point;

        renderPoint.x -= cameraPoint.x;
        renderPoint.y -= cameraPoint.y;
        top.sprite->copyTo(_renderer, renderPoint);

        _priorityQueue.pop();
    }

    SDL_RenderPresent(_renderer);
}
