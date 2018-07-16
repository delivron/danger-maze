#include "hero.h"

using namespace media;
using namespace object;

IHero::IHero(Fraction fraction, AnimationPtr animation, float speed)
    : IDynamicObject(fraction, animation, speed)
{
}

void IHero::setDirection(Direction direction) noexcept {
    IDynamicObject::setDirection(direction);
    _animation->goToAndStop( static_cast<uint32_t>(direction) );
}
