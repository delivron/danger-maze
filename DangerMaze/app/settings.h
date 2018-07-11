#pragma once

#include <string>

namespace app {

    struct DisplaySettings {
        int                         width               = 1024;
        int                         height              = 768;
        bool                        fullscreen          = false;
    };

    struct WorldSettings {
        int                         width               = 25;
        int                         height              = 25;
        int                         enemiesCount        = 5;
        int                         cannonsCount        = 5;
    };

    struct Settings {
        DisplaySettings             display;
        WorldSettings               world;

        static Settings             initializeFromFile(const std::string& path);
    };

}
