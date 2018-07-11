#include <string>

#include "app/settings.h"
#include "app/application.h"

using namespace std;
using namespace app;

const string WINDOW_TITLE       = "Danger Maze";
const string SETTINGS_FILE_PATH = "settings.xml";

int SDL_main(int argc, char* argv[]) {
    /*
        TODO:
        1. размер экрана по умолчанию 1024x768
        2. возможнось запуска без библиотек VS
        3. иконка
        4. README.md
    */

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
