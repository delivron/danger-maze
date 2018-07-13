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
using namespace util;
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
    , _mouseControl(false)
    , _window(nullptr)
    , _camera(nullptr)
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
    int sdlInitResult = SDL_Init(SDL_INIT_VIDEO);
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
    _resourceManager.loadAnimationFromDescription("sprites/player.xml", renderer);

    _field = generateField(settings.world.width, settings.world.height);

    _camera = make_shared<Camera>(
        settings.display.width,
        settings.display.height,
        generateVisibleRect(_field, settings.display.width, settings.display.height)
    );

    // новая позиция камеры, что бы указанная точка была по центру окна
    Position targetPosition = { 0, 0 };
    SDL_Point cameraPosition = convertToSdlPoint( _field->getIsometricCoord(targetPosition) );
    cameraPosition.x -= settings.display.width / 2 - TILE_DESCRIPTION.halfHorizontalDiag;
    cameraPosition.y -= settings.display.height / 2 - TILE_DESCRIPTION.tileY;
    _camera->setPosition(cameraPosition);

    return true;
}

void Application::update() {

}

void Application::render() {
    showField();
    showCursor();

    _renderManager.renderAll(_camera);
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
    _currentPos = _field->getRowCol(event.motion.x, event.motion.y, _camera);
}

void Application::handleMouseButton(const SDL_Event& event) noexcept {
    bool isButtonDown = (event.button.type == SDL_MOUSEBUTTONDOWN);

    if (event.button.button == SDL_BUTTON_MIDDLE) {
        _mouseControl = isButtonDown;
    }
    else if (event.button.button == SDL_BUTTON_RIGHT && isButtonDown) {
        if (_field->isCorrectPosition(_currentPos)) {
            if (_field->getState(_currentPos) == TileState::DEFAULT) {
                _field->setState(_currentPos, TileState::WALL_DEFAULT);
            }
            else if (_field->getState(_currentPos) == TileState::WALL_DEFAULT) {
                _field->setState(_currentPos, TileState::DEFAULT);
            }
        }
    }
    else if (event.button.button == SDL_BUTTON_LEFT && isButtonDown) {
        // go to point
    }
}

FieldPtr Application::generateField(uint32_t width, uint32_t height) const {
    FieldPtr field = make_shared<Field>(
        width,
        height,
        TILE_DESCRIPTION
    );

    field->setState({ 1, 1 }, TileState::FINISH);

    int w = static_cast<int>(width);
    int h = static_cast<int>(height);

    for (int column = 0; column < w; ++column) {
        field->setState({ 0, column }, TileState::WALL_BORDER);
        field->setState({ h - 1, column }, TileState::WALL_BORDER);
    }

    for (int row = 1; row < h - 1; ++row) {
        field->setState({ row, 0 }, TileState::WALL_BORDER);
        field->setState({ row, w - 1 }, TileState::WALL_BORDER);
    }

    return field;
}

void Application::showField() {
    const TileMatrix& tiles = _field->getTiles();
    int width = _field->getWidth();
    int height = _field->getHeight();

    // рендеринг игрового поля
    for (int i = 0; i < height; ++i) {
        const vector<Tile> row = tiles[i];

        for (int j = 0; j < width; ++j) {
            const Tile& tile = row[j];
            string spriteName = getTileName(tile.state);
            SpritePtr sprite = _resourceManager.getSprite(spriteName);

            if (sprite != nullptr) {
                int priority = _field->getPriority({ i, j });
                _renderManager.addToQueue(sprite, tile.drawPoint, priority);
            }
        }
    }
}

void Application::showCursor() {
    bool isCorrectPosition = _field->isCorrectPosition(_currentPos);

    if (isCorrectPosition && _field->isWalkable(_currentPos)) {
        SpritePtr cursorSprite = _resourceManager.getSprite("cursor");
        Coordinate addCoord = _field->getIsometricCoord(_currentPos);
        int priority = _field->getPriority(_currentPos);

        _renderManager.addToQueue(cursorSprite, convertToSdlPoint(addCoord), priority + 1);
    }
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
