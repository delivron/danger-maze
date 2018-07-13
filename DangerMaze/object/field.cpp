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
        0.5f * powf(static_cast<float>(2.0f * _tileDescription.halfHorizontalDiag), 2.0f)
    );

    _tiles.assign(height, vector<Tile>(width));

    for (size_t i = 0; i < _tiles.size(); ++i) {
        vector<Tile>& row = _tiles[i];
        for (size_t j = 0; j < row.size(); ++j) {
            Coordinate spriteCoord = getIsometricCoord(
                static_cast<int>(i),
                static_cast<int>(j)
            );
            row[j].isometricCoord = convertToSdlPoint(spriteCoord);
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

int Field::getPriority(int row, int column) const noexcept {
    const int PRIORITY_STEP = 10;
    return PRIORITY_STEP * (row * getWidth() + column);
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
    float x = (static_cast<float>(column) + 0.5f) * _transformParams.squareSize;
    float y = (static_cast<float>(row) + 0.5f) * _transformParams.squareSize;
    return { x, y };
}

std::pair<int, int> Field::getRowCol(int x, int y, CameraPtr camera) const noexcept {
    SDL_Point cameraPoint = camera->getPosition();
    SDL_Point clickPoint = convertToSdlPoint(
        getCartesianCoord(Coordinate{
            static_cast<float>(x + cameraPoint.x),
            static_cast<float>(y + cameraPoint.y)
        })
    );

    return {
        static_cast<int>(clickPoint.y / _transformParams.squareSize),
        static_cast<int>(clickPoint.x / _transformParams.squareSize)
    };
}

const TileMatrix& Field::getTiles() const noexcept {
    return _tiles;
}

TileState Field::getState(int row, int column) const {
    return _tiles[row][column].state;
}

bool Field::isCorrectPosition(int row, int column) const {
    return row >= 0 && row < getHeight() && column >= 0 && column < getWidth();
}

bool Field::isWalkable(int row, int column) const {
    TileState state = _tiles[row][column].state;
    return state == TileState::DEFAULT || state == TileState::FINISH;
}

void Field::setState(int row, int column, TileState newState) {
    _tiles[row][column].state = newState;
}

string object::getTileName(TileState state) {
    switch (state) {
    case TileState::FINISH:
        return "tile_finish";

    case TileState::DEFAULT:
        return "tile_default";

    case TileState::WALL_BORDER:
        return "wall_border";

    case TileState::WALL_DEFAULT:
        return "wall_default";
    }
    return string{};
}

SDL_Rect object::generateVisibleRect(const FieldPtr field, int windowWidth, int windowHeight) {
    TileDescription tileDescr = field->getTileDescription();
    int fieldLeftX = tileDescr.halfHorizontalDiag * (field->getHeight() - 1);

    return {
        -fieldLeftX - windowWidth / 2,
        -windowHeight / 2 + tileDescr.halfVerticalDiag,
        fieldLeftX + tileDescr.halfHorizontalDiag * (field->getWidth() - 1) + windowWidth,
        tileDescr.halfVerticalDiag * (field->getHeight() - 1)
        + tileDescr.halfVerticalDiag * (field->getWidth() - 1) + windowHeight
    };
}
