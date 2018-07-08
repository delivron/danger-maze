#pragma once

#include <string>

namespace app {

    struct Settings {
        int                         windowWidth         = 1024;
        int                         windowHeight        = 768;
        bool                        fullscreen          = false;

        static Settings             initializeFromFile(const std::string& path);
    };

}
