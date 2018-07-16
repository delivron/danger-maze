#include "dynamic_object.h"

using namespace util;
using namespace media;
using namespace object;

IDynamicObject::IDynamicObject(Fraction fraction, media::AnimationPtr animation, float speed)
    : _fraction(fraction)
    , _beginPosition({0, 0})
    , _endPosition({ 0, 0 })
    , _cartesianCoord({ 0.0f, 0.0f })
    , _speed(speed)
    , _alive(true)
    , _moveFlag(false)
    , _direction(Direction::LEFT)
    , _animation(animation)
{
    if (_animation != nullptr) {
        _animation->play();
    }
}

Fraction IDynamicObject::getFraction() const noexcept {
    return _fraction;
}

Position IDynamicObject::getBeginPosition() const noexcept {
    return _beginPosition;
}

void IDynamicObject::setBeginPosition(const Position& position) noexcept {
    _beginPosition = position;
}

Position IDynamicObject::getEndPosition() const noexcept {
    return _endPosition;
}

void IDynamicObject::setEndPosition(const Position& position) noexcept {
    _endPosition = position;
}

Coordinate IDynamicObject::getCartesianCoord() const noexcept {
    return _cartesianCoord;
}

void IDynamicObject::setCartesianCoord(const Coordinate& cartesianPosition) noexcept {
    _cartesianCoord = cartesianPosition;
}

Direction IDynamicObject::getDirection() const noexcept {
    return _direction;
}

void IDynamicObject::setDirection(Direction direction) noexcept {
    setEndPosition( nextPosition(_beginPosition, direction) );
    _direction = direction;
}

float IDynamicObject::getSpeed() const noexcept {
    return _speed;
}

void IDynamicObject::setSpeed(float speed) noexcept {
    _speed = speed;
}

bool IDynamicObject::isAlive() const noexcept {
    return _alive;
}

void IDynamicObject::setAlive(bool alive) noexcept {
    _alive = alive;
}

bool IDynamicObject::isMove() const noexcept {
    return _moveFlag;
}

void IDynamicObject::setMoveFlag(bool moveFlag) {
    _moveFlag = moveFlag;
}

AnimationPtr IDynamicObject::getAnimation() const noexcept {
    return _animation;
}

void IDynamicObject::update() {
    if (_animation != nullptr) {
        _animation->update();
    }
}
