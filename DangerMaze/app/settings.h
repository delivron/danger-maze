#pragma once

#include <string>

namespace app {

    struct Settings {
        int                         _windowWidth        = 1024;
        int                         _windowHeight       = 768;
        bool                        _fullscreenMode     = false;

        static Settings             initializeFromFile(const std::string& path);
    };

}
