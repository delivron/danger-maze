#include <cmath>

#include "field.h"

using namespace std;
using namespace util;
using namespace object;

Field::Field(uint32_t width, uint32_t height, const TileDescription& tileDescription)
    : _tileDescription(tileDescription)
{
    _transformParams.toCartesianScaleY = 1.0f * _tileDescription.halfHorizontalDiag
        / _tileDescription.halfVerticalDiag;

    _transformParams.toIsometricScaleY = 1.0f * _tileDescription.halfVerticalDiag
        / _tileDescription.halfHorizontalDiag;

    _transformParams.squareSize = sqrtf(
        powf(static_cast<float>(_tileDescription.halfHorizontalDiag), 2.0f)
        + powf(static_cast<float>(_tileDescription.halfVerticalDiag), 2.0f)
    );

    _tiles.assign(height, vector<Tile>(width));

    for (size_t i = 0; i < _tiles.size(); ++i) {
        vector<Tile>& row = _tiles[i];
        for (size_t j = 0; j < row.size(); ++j) {
            row[j].isometricCoord = convertToSdlPoint( getIsometricCoord(i, j) );
        }
    }
}

TileDescription Field::getTileDescription() const noexcept {
    return _tileDescription;
}

int Field::getWidth()const noexcept {
    return (_tiles.size() == 0) ? 0 : static_cast<int>(_tiles[0].size());
}

int Field::getHeight()const noexcept {
    return static_cast<int>(_tiles.size());
}

Coordinate Field::getIsometricCoord(const Coordinate& cartesianCoord) const noexcept {
    Coordinate rotatedCoord = rotate(cartesianCoord, QUATER_PI);
    return scale(
        rotatedCoord, 1.0f, _transformParams.toIsometricScaleY
    );
}

Coordinate Field::getIsometricCoord(int row, int column) const noexcept {
    float isometricX = static_cast<float>(column - row) * _tileDescription.halfHorizontalDiag;
    float isometricY = static_cast<float>(column + row) * _tileDescription.halfVerticalDiag;
    return {
        isometricX - _tileDescription.tileX,
        isometricY - _tileDescription.tileY + _tileDescription.halfVerticalDiag
    };
}

Coordinate Field::getCartesianCoord(const Coordinate& isometricCoord) const noexcept {
    Coordinate scaledCoord = scale(isometricCoord, 1.0f, _transformParams.toCartesianScaleY);
    return rotate(scaledCoord, -QUATER_PI);
}

Coordinate Field::getCartesianCoord(int row, int column) const noexcept {
    float x = ((float)column + 0.5f) * _transformParams.squareSize;
    float y = ((float)row + 0.5f) * _transformParams.squareSize;
    return { x, y };
}

const TileMatrix& Field::getTiles() const noexcept {
    return _tiles;
}

TileState Field::getState(int row, int column) const {
    return _tiles[row][column].state;
}

void Field::setState(int row, int column, TileState newState, bool checkOverlaping) {
    if (checkOverlaping && newState == TileState::WALL_BIG) {
        if (row > 0 && column > 0) {
            TileState leftUpState = getState(row - 1, column - 1);
            if (leftUpState == TileState::DEFAULT || leftUpState == TileState::FINISH) {
                newState = TileState::WALL_BIG_ALPHA;
            }
        }
        else if (row > 0) {
            TileState upState = getState(row - 1, column);
            if (upState == TileState::DEFAULT || upState == TileState::FINISH) {
                newState = TileState::WALL_BIG_ALPHA;
            }
        }
        else if (column > 0) {
            TileState leftState = getState(row, column - 1);
            if (leftState == TileState::DEFAULT || leftState == TileState::FINISH) {
                newState = TileState::WALL_BIG_ALPHA;
            }
        }
    }

    _tiles[row][column].state = newState;
}

string object::getTileName(TileState state) {
    switch (state) {
    case TileState::FINISH:
        return "tile_finish";

    case TileState::DEFAULT:
        return "tile_default";

    case TileState::WALL_SMALL:
        return "wall_small";

    case TileState::WALL_BIG:
        return "wall_big";

    case TileState::WALL_BIG_ALPHA:
        return "wall_big_alpha";

    case TileState::WALL_BIG_ALPHA_LEFT:
        return "wall_big_alpha_left";

    case TileState::WALL_BIG_ALPHA_RIGHT:
        return "wall_big_alpha_right";
    }
    return string{};
}
