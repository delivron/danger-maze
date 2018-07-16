#pragma once

namespace object {

    enum class Direction {
        DOWN                        = 0,
        RIGHT                       = 1,
        UP                          = 2,
        LEFT                        = 3,
    };

    struct Position {
        int                         row                 = -1;
        int                         column              = -1;
    };

    bool operator==(const Position& lhs, const Position& rhs);
    bool operator!=(const Position& lhs, const Position& rhs);

    Position nextPosition(Position position, Direction direction);

}
