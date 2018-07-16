#pragma once

#include <deque>
#include <memory>

#include "hero.h"

namespace object {

    class Player : public IHero {
    public:
        Player(media::AnimationPtr animation, float speed);

        void                        update() override;
        void                        setPath(const std::deque<Direction>& path);
        void                        clearPath();

    private:
        std::deque<Direction>       _path;
    };

    using PlayerPtr                 = std::shared_ptr<Player>;

}
