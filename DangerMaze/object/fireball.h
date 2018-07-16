#pragma once

#include "dynamic_object.h"

namespace object {

    class Fireball : public IDynamicObject {
    public:
        Fireball(media::AnimationPtr animation, float speed);

        void                        update() override;
    };

}