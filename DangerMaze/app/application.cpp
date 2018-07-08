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

void Application::run() noexcept {
    _running = true;
}

bool Application::initialize(const string& title, const Settings& settings) {
    int initResult = SDL_Init(SDL_INIT_EVERYTHING);
    if (initResult  < 0) {
        return false;
    }
    
    _window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        settings._windowWidth,
        settings._windowHeight,
        SDL_WINDOW_SHOWN
    );
    if (_window == nullptr) {
        return false;
    }

    return true;
}

void Application::simulate() {

}

void Application::render() {

}

void Application::handleEvent(const SDL_Event& event) {
    if (event.type == SDL_QUIT) {
        _running = false;
    }
}

void Application::cleanup() {
    SDL_DestroyWindow(_window);
    SDL_Quit();
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
