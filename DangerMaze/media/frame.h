#pragma once

#include <cstdint>

#include "sprite.h"

namespace media {

    struct Frame {
        SpritePtr                   sprite      = nullptr;
        uint32_t                    delayMs     = 100;
    };

}
