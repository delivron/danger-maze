#pragma once

#include <memory>
#include <utility>

#include "hero.h"

namespace object {

    // Область, в виде прямоугольника: координаты левого верхнего и правого нижнего углов.
    using ControlZone               = std::pair<Position, Position>;

    class Enemy : public IHero {
    public:
        Enemy(media::AnimationPtr animation, float speed);

        ControlZone                 getControlZone() const noexcept;
        void                        setControlZone(const ControlZone& zone) noexcept;
        void                        update() override;
        void                        onHitWithBarrier() override;

    private:
        ControlZone                 _controlZone;
    };

    using EnemyPtr                  = std::shared_ptr<Enemy>;

}
