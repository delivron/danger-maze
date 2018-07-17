#include "enemy.h"

using namespace media;
using namespace object;

Enemy::Enemy(media::AnimationPtr animation, float speed)
    : IHero(Fraction::ENEMY, animation, speed)
{
}

ControlZone Enemy::getControlZone() const noexcept {
    return _controlZone;
}

void Enemy::setControlZone(const ControlZone& zone) noexcept {
    _controlZone = zone;
}

void Enemy::update() {
    if (isMove()) {
        return;
    }

    Position currentPos = getBeginPosition();
    Position p1 = _controlZone.first;
    Position p2 = _controlZone.second;
    
    Direction nextDir = _direction;

    if (currentPos.column < p1.column) {
        nextDir = Direction::RIGHT;
    }
    else if (currentPos.column > p2.column) {
        nextDir = Direction::LEFT;
    }
    else if (currentPos.row < p1.row) {
        nextDir = Direction::DOWN;
    }
    else if (currentPos.row > p2.row) {
        nextDir = Direction::UP;
    }
    else {
        if (isHorizontalDirection(_direction)) {
            if (currentPos.column == p1.column && _direction == Direction::LEFT) {
                nextDir = Direction::RIGHT;
            }
            else if (currentPos.column == p2.column && _direction == Direction::RIGHT) {
                nextDir = Direction::LEFT;
            }
        }
        else { // vertical direction
            if (currentPos.row == p1.row && _direction == Direction::UP) {
                nextDir = Direction::DOWN;
            }
            else if (currentPos.row == p2.row && _direction == Direction::DOWN) {
                nextDir = Direction::UP;
            }
        }
    }

    startMotion(nextDir);
}

void Enemy::onHitWithBarrier() {
    resetMotion();
    setDirection( invertDirection(_direction) );
}
