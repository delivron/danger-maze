#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>
#include <utility>

#include "../media/sprite.h"

#include "../object/camera.h"

#include "../util/transformation.h"

namespace object {
    
    enum class TileState {
        DEFAULT,
        FINISH,
        WALL_BORDER,
        WALL_DEFAULT
    };
    
    struct Tile {
        TileState                   state               = TileState::DEFAULT;
        SDL_Point                   isometricCoord;
    };

    struct TileDescription {
        int                         spriteWidth         = 0;
        int                         spriteHeight        = 0;
        int                         halfHorizontalDiag  = 0;
        int                         halfVerticalDiag    = 0;
        int                         tileX               = 0;
        int                         tileY               = 0;
    };

    // заранее вычисленные коэфициенты для преобразования координат
    // из декартовой в изометрическую и наоборот
    struct TransformationParameters {
        float                       toIsometricScaleY   = 1.0f;
        float                       toCartesianScaleY   = 1.0f;
        float                       squareSize          = 1.0f;
    };

    using TileMatrix                = std::vector<std::vector<Tile>>;

    class Field {
    public:
        Field(uint32_t width, uint32_t height, const TileDescription& tileDescription);
        
        TileDescription             getTileDescription() const noexcept;
        int                         getWidth() const noexcept;
        int                         getHeight() const noexcept;
        int                         getPriority(int row, int column) const noexcept;
        util::Coordinate            getIsometricCoord(const util::Coordinate& cartesianCoord) const noexcept;
        util::Coordinate            getIsometricCoord(int row, int column) const noexcept;
        util::Coordinate            getCartesianCoord(const util::Coordinate& isometricCoord) const noexcept;
        util::Coordinate            getCartesianCoord(int row, int column) const noexcept;
        std::pair<int, int>         getRowCol(int x, int y, CameraPtr camera) const noexcept;
        const TileMatrix&           getTiles() const noexcept;
        TileState                   getState(int row, int column) const;
        bool                        isCorrectPosition(int row, int column) const;
        bool                        isWalkable(int row, int column) const;
        void                        setState(int row, int col, TileState newState);

    private:
        TransformationParameters    _transformParams;
        TileDescription             _tileDescription;
        TileMatrix                  _tiles;
    };

    using FieldPtr                  = std::shared_ptr<Field>;

    std::string                     getTileName(TileState state);
    SDL_Rect                        generateVisibleRect(const FieldPtr field, int windowWidth, int windowHeight);

}
