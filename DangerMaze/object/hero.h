#pragma once

#include "dynamic_object.h"

namespace object {

    class IHero : public IDynamicObject {
    public:
        IHero(Fraction fraction, media::AnimationPtr animation, float speed);

        void                        setDirection(Direction direction) noexcept;
        void                        update() = 0;
    };

    using IHeroPtr                  = std::shared_ptr<IHero>;

}
