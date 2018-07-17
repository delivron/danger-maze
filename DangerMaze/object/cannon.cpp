#include "cannon.h"

using namespace media;
using namespace object;

Cannon::Cannon(const AnimationPtr animation, clock_t delayMs)
    : IStaticObject(animation)
    , _position({ 0, 0 })
    , _lastTimeMs(clock())
    , _currentMs(0)
    , _delayMs(delayMs)
{
    const uint32_t DIRECTION_COUNT = 4;
    _statesCount = animation->getFramesCount() / DIRECTION_COUNT;
}

void Cannon::setDirection(Direction direction) {
    IStaticObject::setDirection(direction);
    updateFrame();
}

Position Cannon::getPosition() const noexcept {
    return _position;
}

void Cannon::setPosition(Position position) {
    _position = position;
}

void Cannon::update() {
    clock_t currentTime = clock();

    if (_currentMs < _delayMs) {
        clock_t deltaTime = currentTime - _lastTimeMs;
        _currentMs += deltaTime;
        updateFrame();
    }

    _lastTimeMs = currentTime;
}

bool Cannon::tryShoot() {
    if (_currentMs >= _delayMs) {
        _currentMs = 0;
        updateFrame();
        return true;
    }
    return false;
}

void Cannon::updateFrame() {
    uint32_t state = 0;
    if (_statesCount > 0) {
        clock_t stepMs = _delayMs / _statesCount;
        state = _currentMs / stepMs;
        if (state >= _statesCount) {
            state = _statesCount - 1;
        }
    }

    uint32_t frame = static_cast<uint32_t>(_direction) * _statesCount + state;
    _animation->goToAndStop(frame);
}
