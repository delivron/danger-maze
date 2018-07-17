#include "player.h"

using namespace std;
using namespace object;

Player::Player(media::AnimationPtr animation, float speed)
    : IHero(Fraction::PLAYER, animation, speed)
{    
    setDirection(Direction::LEFT);
}

deque<Direction> Player::getPath() const noexcept {
    return _path;
}

void Player::setPath(const deque<Direction>& path) {
    _path = path;
}

void Player::clearPath() {
    _path.clear();
}

void Player::update() {
    if (!_moveFlag && !_path.empty()) {
        startMotion( _path.front() );
        _path.pop_front();
    }
}

void Player::onHitWithBarrier() {
    resetMotion();
    clearPath();
}

void Player::onHitWithObject(Fraction objectFraction) {
    if (objectFraction == Fraction::ENEMY) {
        onHitWithBarrier();
        setAlive(false);
    }
}