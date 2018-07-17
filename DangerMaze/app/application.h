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
#include "../object/cannon.h"
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
        void                        initializeObjects();
        void                        addFieldToPriorityTree(PriorityTree& tree) const;
        void                        addMarkersToPriorityTree(PriorityTree& tree) const;
        void                        addObjectsToPriorityTree(PriorityTree& tree) const;
        void                        updateLevelState();
        void                        addCannon(const object::Position& addPos, object::Direction direction);
        void                        addFireball(const object::Position& addPos, object::Direction direction);
        void                        addEnemy(
                                        const object::Position& addPos,
                                        const object::Position& p1,
                                        const object::Position& p2,
                                        object::Direction direction
                                    );

        using DynamicObjects        = std::vector<object::IDynamicObjectPtr>;
        using Cannons               = std::vector<object::CannonPtr>;

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
        DynamicObjects              _objects;
        Cannons                     _cannons;
        clock_t                     _lastUpdateTime;

        static const object::TileDescription    TILE_DESCRIPTION;
        static const SDL_Color                  BACKGROUND_COLOR;
        static const uint32_t                   FIELD_SIZE;
        static const int                        CANNON_DELAY_MS_MIN;
        static const int                        CANNON_DELAY_MS_MAX;
        static const float                      PLAYER_SPEED;
        static const float                      ENEMY_SPEED;
        static const float                      FIREBALL_SPEED;
    };

    void                            loop(Application& application);

}
