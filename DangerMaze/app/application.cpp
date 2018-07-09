#include <iostream>

#include "SDL_image.h"

#include "application.h"

#define CHECK_SDL_RESULT(condition, functionName)                           \
if (condition) {                                                            \
    cout << "Error [" << functionName << "]: " << SDL_GetError() << endl;   \
    return false;                                                           \
}

using namespace std;
using namespace app;

Application::Application() noexcept
    : _running(false)
    , _window(nullptr)
{
}

Application::~Application() {
    cleanup();
}

bool Application::isRunning() const noexcept {
    return _running;
}

void Application::setRunning(bool running) noexcept {
    _running = running;
}

bool Application::initialize(const string& title, const Settings& settings) {
    int sdlInitResult = SDL_Init(SDL_INIT_EVERYTHING);
    CHECK_SDL_RESULT(sdlInitResult < 0, "SDL_Init");
    
    int imgInitResult = IMG_Init(IMG_INIT_PNG);
    CHECK_SDL_RESULT(imgInitResult < 0, "IMG_Init");

    _window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        settings.windowWidth,
        settings.windowHeight,
        SDL_WINDOW_SHOWN
    );
    CHECK_SDL_RESULT(_window == nullptr, "SDL_CreateWindow");

    if (settings.fullscreen) {
        SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    
    _renderer = SDL_CreateRenderer(
        _window,
        -1,                         // ������� ������� ��� ���������� �������������
        SDL_RENDERER_ACCELERATED |  // ���������� ���������
        SDL_RENDERER_PRESENTVSYNC   // ������������ �������������
    );
    CHECK_SDL_RESULT(_renderer == nullptr, "SDL_CreateRenderer");

    _sprite = obj::Sprite::createFromPng("resources/sprites/default_tile.png", _renderer); // DELETE

    return true;
}

void Application::simulate() {

}

void Application::render() {
    SDL_SetRenderDrawColor(_renderer, 155, 221, 255, 255);
    SDL_Rect rect1 = { 0, 0, 70, 45 };
    SDL_Rect rect2 = { 70, 0, 70, 45 };

    SDL_RenderClear(_renderer);
    SDL_RenderCopy(_renderer, _sprite->getTexture(), nullptr, &rect1);
    SDL_RenderCopy(_renderer, _sprite->getTexture(), nullptr, &rect2);
    SDL_RenderPresent(_renderer);
}

void Application::handleEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_QUIT:
        setRunning(false);
        break;

    case SDL_KEYUP:
        handleKeyUp(event);
        break;
    }
}

void Application::cleanup() {
    SDL_DestroyWindow(_window);
    SDL_DestroyRenderer(_renderer);
    SDL_Quit();
    IMG_Quit();
}

void Application::handleKeyUp(const SDL_Event& event) noexcept {
    SDL_Scancode scancode = event.key.keysym.scancode;
    if (scancode == SDL_SCANCODE_ESCAPE) {
        setRunning(false);
    }
}

void app::loop(Application& app) {
    while (app.isRunning()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app.handleEvent(event);
        }

        app.simulate();
        app.render();
    }
}
