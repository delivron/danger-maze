#pragma once

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "../media/sprite.h"

#include "../util/transformation.h"

namespace object {
    
    enum class TileState {
        DEFAULT,
        FINISH,
        WALL_SMALL,
        WALL_BIG,
        WALL_BIG_ALPHA,
        WALL_BIG_ALPHA_LEFT,  // лева€ бокова€ часть прозрачна€, а права€ нет
        WALL_BIG_ALPHA_RIGHT, // права€ бокова€ часть прозрачна€, а лева€ нет
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

    // заранее вычисленные коэфициенты дл€ преобразовани€ координат
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
        util::Coordinate            getIsometricCoord(const util::Coordinate& cartesianCoord) const noexcept;
        util::Coordinate            getIsometricCoord(int row, int column) const noexcept;
        util::Coordinate            getCartesianCoord(const util::Coordinate& isometricCoord) const noexcept;
        util::Coordinate            getCartesianCoord(int row, int column) const noexcept;
        const TileMatrix&           getTiles() const noexcept;
        TileState                   getState(int row, int column) const;

        // ”становить состо€ние клетки на newState по координатам (row, col).
        // ”сли параметр checkOverlaping включЄн, то стены,
        // которые перекрывают плитки дл€ хождени€, станов€тс€ прозрачными
        void                        setState(int row, int col, TileState newState, bool checkOverlaping = false);

    private:
        TransformationParameters    _transformParams;
        TileDescription             _tileDescription;
        TileMatrix                  _tiles;
    };

    using FieldPtr                  = std::shared_ptr<Field>;

    std::string                     getTileName(TileState state);

}
