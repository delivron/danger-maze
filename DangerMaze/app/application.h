#pragma once

#include <string>

#include "SDL.h"

#include "settings.h"

#include "../object/field.h"
#include "../object/camera.h"

#include "../media/render_manager.h"
#include "../media/resource_manager.h"

namespace app {

    class Application {
    public:
        Application() noexcept;
        ~Application();

        bool                        isRunning() const noexcept;
        void                        setRunning(bool running) noexcept;
        bool                        initialize(const std::string& title, const Settings& settings);
        void                        update();
        void                        render();
        void                        handleEvent(const SDL_Event& event);
        void                        cleanup();

    private:
        void                        handleKeyUp(const SDL_Event& event) noexcept;
        void                        handleMouseMotion(const SDL_Event& event) noexcept;
        void                        handleMouseButton(const SDL_Event& event) noexcept;
        object::FieldPtr            generateField(uint32_t width, uint32_t height) const;
        void                        showField();
        void                        showCursor();

        bool                        _running;
        bool                        _mouseControl;
        SDL_Window*                 _window;
        media::ResourceManager      _resourceManager;
        media::RenderManager        _renderManager;
        object::CameraPtr           _camera;
        object::FieldPtr            _field;
        std::pair<int, int>         _currentRowCol;

        static const SDL_Color                  BACKGROUND_COLOR;
        static const object::TileDescription    TILE_DESCRIPTION;
    };

    void                            loop(Application& application);

}
