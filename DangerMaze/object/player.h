#pragma once

#include <deque>
#include <memory>

#include "hero.h"

namespace object {

    class Player : public IHero {
    public:
        Player(const media::AnimationPtr animation, float speed);

        std::deque<Direction>       getPath() const noexcept;
        void                        setPath(const std::deque<Direction>& path);
        void                        clearPath();
        void                        update() override;
        void                        onHitWithBarrier() override;
        void                        onHitWithObject(Fraction objectFraction) override;

    private:
        std::deque<Direction>       _path;
    };

    using PlayerPtr                 = std::shared_ptr<Player>;

}
