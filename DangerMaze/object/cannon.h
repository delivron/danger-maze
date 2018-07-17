#pragma once

#include <ctime>

#include "static_object.h"

namespace object {

    class Cannon : public IStaticObject {
    public:
        Cannon(const media::AnimationPtr animation, clock_t delayMs);

        void                        setDirection(Direction direction) override;
        Position                    getPosition() const noexcept;
        void                        setPosition(Position position);
        void                        update() override;
        bool                        tryShoot();

    private:
        void                        updateFrame();

        uint32_t                    _statesCount;
        Position                    _position;
        clock_t                     _lastTimeMs;
        clock_t                     _currentMs;
        clock_t                     _delayMs;
    };

    using CannonPtr                 = std::shared_ptr<Cannon>;

}
