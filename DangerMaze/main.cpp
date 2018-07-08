#include <string>

#include "app/settings.h"
#include "app/application.h"

using namespace std;
using namespace app;

const string WINDOW_TITLE = "Danger Maze";

int SDL_main(int argc, char* argv[]) {
    Application app;
    Settings settings;

    bool initSuccess = app.initialize(WINDOW_TITLE, settings);
    if (!initSuccess) {
        return -1;
    }

    app.run();

    while (app.isRunning()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app.handleEvent(event);
        }

        app.simulate();
        app.render();
    }

    return 0;
}
