#pragma once

#include <ctime>

#include "../object/field.h"
#include "../object/camera.h"
#include "../object/dynamic_object.h"

namespace app {

    const clock_t                   MS_IN_SECOND        = 1000;

    void                            moveObject(object::IDynamicObjectPtr object, object::FieldPtr field, clock_t deltaTimeMs);

}
