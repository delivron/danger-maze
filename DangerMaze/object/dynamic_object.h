#pragma once

#include <memory>
#include <cstdint>

#include "position.h"

#include "../media/animation.h"

#include "../util/transformation.h"

namespace object {

    enum class Fraction {
        ENEMY                       = 0,
        PLAYER                      = 1,
    };

    class IDynamicObject {
    public:
        IDynamicObject(Fraction fraction, media::AnimationPtr animation, float speed);

        Fraction                    getFraction() const noexcept;
        Position                    getBeginPosition() const noexcept;
        Position                    getEndPosition() const noexcept;
        void                        setPosition(const Position& position) noexcept;
        util::Coordinate            getCartesianCoord() const noexcept;
        void                        setCartesianCoord(const util::Coordinate& cartesianPosition) noexcept;
        Direction                   getDirection() const noexcept;
        virtual void                setDirection(Direction direction) noexcept;
        float                       getSpeed() const noexcept;
        void                        setSpeed(float speed) noexcept;
        bool                        isAlive() const noexcept;
        void                        setAlive(bool alive) noexcept;
        bool                        isMove() const noexcept;
        void                        setMoveFlag(bool moveFlag);
        media::AnimationPtr         getAnimation() const noexcept;
        void                        startMotion(Direction direction);
        void                        finishMotion();
        void                        resetMotion();
        virtual void                update() = 0;
        virtual void                onHitWithBarrier() = 0;
        virtual void                onHitWithObject(Fraction objectFraction);

    protected:
        Fraction                    _fraction;
        Position                    _beginPosition;
        Position                    _endPosition;
        util::Coordinate            _cartesianCoord;
        Direction                   _direction;
        float                       _speed;
        bool                        _alive;
        bool                        _moveFlag;
        const media::AnimationPtr   _animation;
    };

    using IDynamicObjectPtr         = std::shared_ptr<IDynamicObject>;

}
