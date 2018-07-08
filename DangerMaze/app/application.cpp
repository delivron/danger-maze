#include <iostream>

#include "application.h"

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
    int initResult = SDL_Init(SDL_INIT_EVERYTHING);
    if (initResult  < 0) {
        cout << "Error [SDL_Init]: " << SDL_GetError() << endl;
        return false;
    }
    
    _window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        settings.windowWidth,
        settings.windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (_window == nullptr) {
        cout << "Error [SDL_CreateWindow]: " << SDL_GetError() << endl;
        return false;
    }

    if (settings.fullscreen) {
        SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    
    _renderer = SDL_CreateRenderer(
        _window,
        -1,                         // выбрать драйвер для рендеринга автоматически
        SDL_RENDERER_ACCELERATED |  // аппаратное ускорение
        SDL_RENDERER_PRESENTVSYNC   // вертикальная синхронизация
    );
    if (_renderer == nullptr) {
        cout << "Error [SDL_CreateRenderer]: " << SDL_GetError() << endl;
        return false;
    }

    return true;
}

void Application::simulate() {

}

void Application::render() {

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
