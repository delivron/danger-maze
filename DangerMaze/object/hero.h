#pragma once

#include "dynamic_object.h"

namespace object {

    class IHero : public IDynamicObject {
    public:
        IHero(Fraction fraction, const media::AnimationPtr animation, float speed);

        void                        setDirection(Direction direction) override;
    };

    using IHeroPtr                  = std::shared_ptr<IHero>;

}
