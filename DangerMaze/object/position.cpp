#include "position.h"

using namespace object;

bool object::operator==(const Position& lhs, const Position& rhs) {
    return lhs.row == rhs.row && lhs.column == rhs.column;
}

bool object::operator!=(const Position& lhs, const Position& rhs) {
    return lhs.row != rhs.row || lhs.column != rhs.column;
}

Position object::nextPosition(Position position, Direction direction) {
    switch (direction) {
    case Direction::DOWN:
        ++position.row;
        break;
    case Direction::UP:
        --position.row;
        break;
    case Direction::LEFT:
        --position.column;
        break;
    case Direction::RIGHT:
        ++position.column;
        break;
    }
    return position;
}
