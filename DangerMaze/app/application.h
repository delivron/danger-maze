#pragma once

#include <string>

#include "SDL.h"

#include "settings.h"

namespace app {

    class Application {
    public:
        Application() noexcept;
        ~Application();

        bool                        isRunning() const noexcept;
        void                        run() noexcept;
        bool                        initialize(const std::string& title, const Settings& settings);
        void                        simulate();
        void                        render();
        void                        handleEvent(const SDL_Event& event);
        void                        cleanup();

    private:
        bool                        _running;
        SDL_Window*                 _window;
    };

}
