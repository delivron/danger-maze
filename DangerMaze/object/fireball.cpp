#include "fireball.h"

using namespace media;
using namespace object;

Fireball::Fireball(AnimationPtr animation, float speed)
    : IDynamicObject(Fraction::ENEMY, animation, speed)
{    
}

void Fireball::update() {
    _animation->update();

    if (!_moveFlag) {
        startMotion(_direction);
    }
}

void Fireball::onHitWithBarrier() {
    setAlive(false);
}
