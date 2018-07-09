#pragma once

#include <string>

#include "SDL.h"

#include "settings.h"

#include "../obj/sprite.h"

namespace app {

    class Application {
    public:
        Application() noexcept;
        ~Application();

        bool                        isRunning() const noexcept;
        void                        setRunning(bool running) noexcept;
        bool                        initialize(const std::string& title, const Settings& settings);
        void                        simulate();
        void                        render();
        void                        handleEvent(const SDL_Event& event);
        void                        cleanup();

    private:
        static const Uint8          _backgroundColor[4];

        void                        handleKeyUp(const SDL_Event& event) noexcept;

        bool                        _running;
        SDL_Window*                 _window;
        SDL_Renderer*               _renderer;
    };

    void                            loop(Application& application);

}
