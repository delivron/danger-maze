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

void Player::setPath(const std::deque<Direction>& path) {
    _path = path;
}

void Player::clearPath() {
    _path.clear();
}

void Player::update() {
    if (!_moveFlag && !_path.empty()) {
        setDirection( _path.front() );
        _path.pop_front();
        _moveFlag = true;
    }
}
