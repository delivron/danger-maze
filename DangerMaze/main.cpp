#include <string>

#include "app/settings.h"
#include "app/application.h"

using namespace std;
using namespace app;

const string WINDOW_TITLE       = "Danger Maze";
const string SETTINGS_FILE_PATH = "settings.xml";

int SDL_main(int argc, char* argv[]) {
    Settings settings = Settings::initializeFromFile(SETTINGS_FILE_PATH);
    Application app;

    bool initSuccess = app.initialize(WINDOW_TITLE, settings);
    if (!initSuccess) {
        return EXIT_FAILURE;
    }

    app.setRunning(true);
    loop(app);

    return EXIT_SUCCESS;
}
