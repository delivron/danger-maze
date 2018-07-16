#pragma once

#include <ctime>
#include <string>
#include <vector>

#include "SDL.h"

#include "settings.h"
#include "priority_tree.h"

#include "../object/field.h"
#include "../object/camera.h"
#include "../object/player.h"
#include "../object/dynamic_object.h"

#include "../media/render_manager.h"
#include "../media/resource_manager.h"

namespace app {

    enum class LevelState {
        PLAYING,
        COMPLETED,
        GAME_OVER,
    };

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
        void                        renderInCenter(const media::SpritePtr sprite);
        void                        handleKeyUp(const SDL_Event& event) noexcept;
        void                        handleMouseMotion(const SDL_Event& event) noexcept;
        void                        handleMouseButton(const SDL_Event& event) noexcept;
        void                        handleLeftMouseButton() noexcept;
        void                        handleRightMouseButton() noexcept;
        void                        generateField(uint32_t width, uint32_t height);
        void                        addFieldToPriorityTree(PriorityTree& tree) const;
        void                        addMarkersToPriorityTree(PriorityTree& tree) const;
        void                        addObjectsToPriorityTree(PriorityTree& tree) const;
        void                        updateLevelState();

        using Objects               = std::vector<object::IDynamicObjectPtr>;

        bool                        _running;
        bool                        _mouseControl;
        LevelState                  _state;
        SDL_Window*                 _window;
        media::ResourceManager      _resourceManager;
        media::RenderManager        _renderManager;
        object::CameraPtr           _camera;
        object::FieldPtr            _field;
        object::PlayerPtr           _player;
        object::Position            _finishPos;
        object::Position            _currentPos;
        Objects                     _objects;
        clock_t                     _lastUpdateTime;

        static const SDL_Color                  BACKGROUND_COLOR;
        static const object::TileDescription    TILE_DESCRIPTION;
    };

    void                            loop(Application& application);

}
