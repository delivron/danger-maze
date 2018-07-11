#include <iostream>

#include "SDL_image.h"

#include "application.h"

#define CHECK_SDL_RESULT(condition, functionName)                           \
if (condition) {                                                            \
    cout << "Error [" << functionName << "]: " << SDL_GetError() << endl;   \
    return false;                                                           \
}

using namespace std;
using namespace app;
using namespace media;
using namespace object;

const SDL_Color Application::BACKGROUND_COLOR = { 155, 221, 255, 255 };

const TileDescription Application::TILE_DESCRIPTION = {
    70, // spriteWidth
    81, // spriteHeight
    35, // halfHorizontalDiag
    20, // halfVerticalDiag
    35, // tileX
    55, // tileY
};

Application::Application() noexcept
    : _running(false)
    , _window(nullptr)
    , _field(nullptr)
{
}

Application::~Application() {
    cleanup();
}

bool Application::isRunning() const noexcept {
    return _running;
}

void Application::setRunning(bool running) noexcept {
    _running = running;
}

bool Application::initialize(const string& title, const Settings& settings) {
    int sdlInitResult = SDL_Init(SDL_INIT_EVERYTHING);
    CHECK_SDL_RESULT(sdlInitResult < 0, "SDL_Init");

    int imgInitResult = IMG_Init(IMG_INIT_PNG);
    CHECK_SDL_RESULT(imgInitResult < 0, "IMG_Init");

    _window = SDL_CreateWindow(
        title.c_str(),
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        settings.display.width,
        settings.display.height,
        SDL_WINDOW_SHOWN
    );
    CHECK_SDL_RESULT(_window == nullptr, "SDL_CreateWindow");

    if (settings.display.fullscreen) {
        SDL_SetWindowFullscreen(_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }

    _renderManager = RenderManager(_window);
    _renderManager.setColor(BACKGROUND_COLOR);

    SDL_Renderer* renderer = _renderManager.getRenderer();
    CHECK_SDL_RESULT(renderer == nullptr, "SDL_CreateRenderer");

    _resourceManager.loadSpriteFromDescription("sprites/tiles.xml", renderer);

    _field = generateField(settings.world.width, settings.world.height);

    _camera = make_shared<Camera>(
        settings.display.width,
        settings.display.height,
        generateVisibleRect(_field, settings.display.width, settings.display.height)
    );

    return true;
}

void Application::update() {

}

void Application::render() {
    const TileMatrix& tiles = _field->getTiles();
    int priority = 0;

    for (const vector<Tile>& row : tiles) {
        for (const Tile& tile : row) {
            string spriteName = getTileName(tile.state);
            SpritePtr sprite = _resourceManager.getSprite(spriteName);

            if (sprite != nullptr) {
                _renderManager.addToQueue(sprite, tile.isometricCoord, priority);
            }
        }
        ++priority;
    }

    _renderManager.renderAll(_camera);
    _renderManager.clearQueue();
}

void Application::handleEvent(const SDL_Event& event) {
    switch (event.type) {
    case SDL_QUIT:
        setRunning(false);
        break;

    case SDL_KEYUP:
        handleKeyUp(event);
        break;

    case SDL_MOUSEBUTTONDOWN:
    case SDL_MOUSEBUTTONUP:
        handleMouseButton(event);
        break;

    case SDL_MOUSEMOTION:
        handleMouseMotion(event);
        break;
    }
}

void Application::cleanup() {
    _resourceManager.cleanup();

    SDL_DestroyWindow(_window);
    SDL_Quit();
    IMG_Quit();
}

void Application::handleKeyUp(const SDL_Event& event) noexcept {
    SDL_Scancode scancode = event.key.keysym.scancode;
    if (scancode == SDL_SCANCODE_ESCAPE) {
        setRunning(false);
    }
}

void Application::handleMouseMotion(const SDL_Event& event) noexcept {
    if (_mouseControl) {
        _camera->move(event.motion.xrel, event.motion.yrel);
    }
}

void Application::handleMouseButton(const SDL_Event& event) noexcept {
    if (event.button.button == SDL_BUTTON_MIDDLE) {
        _mouseControl = (event.button.type == SDL_MOUSEBUTTONDOWN);
    }
}

FieldPtr Application::generateField(uint32_t width, uint32_t height) const {
    FieldPtr field = make_shared<Field>(
        width,
        height,
        TILE_DESCRIPTION
    );

    for (uint32_t column = 0; column < width; ++column) {
        field->setState(0, column, TileState::WALL_BIG, true);
        field->setState(height - 1, column, TileState::WALL_BIG, true);
    }

    for (uint32_t row = 1; row < height; ++row) {
        field->setState(row, 0, TileState::WALL_BIG, true);
        field->setState(row, height - 1, TileState::WALL_BIG, true);
    }

    return field;
}

void app::loop(Application& app) {
    while (app.isRunning()) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            app.handleEvent(event);
        }

        app.update();
        app.render();
    }
}
