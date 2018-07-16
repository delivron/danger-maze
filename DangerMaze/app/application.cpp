#include <utility>
#include <iostream>

#include "SDL_image.h"

#include "simulation.h"
#include "application.h"
#include "path_searching.h"

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
    60  // tileY
};

Application::Application() noexcept
    : _running(false)
    , _mouseControl(false)
    , _state(LevelState::PLAYING)
    , _window(nullptr)
    , _camera(nullptr)
    , _field(nullptr)
    , _lastUpdateTime(clock())
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
    _resourceManager.loadSpriteFromDescription("sprites/text.xml", renderer);
    _resourceManager.loadSpriteFromDescription("sprites/markers.xml", renderer);
    _resourceManager.loadAnimationFromDescription("sprites/player.xml", renderer);

    generateField(settings.world.width, settings.world.height);

    _camera = make_shared<Camera>(
        settings.display.width,
        settings.display.height,
        generateVisibleRect(_field, settings.display.width, settings.display.height)
    );

    Position playerPos = { _field->getHeight() - 2, _field->getWidth() - 2 };
    _player = make_shared<Player>(_resourceManager.getAnimation("player"), 75.0f);
    addObject(_field, _player, playerPos);
    _objects.push_back(_player);

    // ����� ������� ������, ��� �� ��������� ����� ���� �� ������ ����
    SDL_Point cameraPosition = convertToSdlPoint( _field->getIsometricCoord(playerPos) );
    cameraPosition.x -= settings.display.width / 2;
    cameraPosition.y -= settings.display.height / 2;
    _camera->setPosition(cameraPosition);

    return true;
}

void Application::update() {
    clock_t currentTime = clock();
    clock_t deltaTimeMs = currentTime - _lastUpdateTime;

    for (size_t i = 0; i < _objects.size(); ++i) {
        IDynamicObjectPtr object = _objects[i];

        if (object == nullptr) {
            continue;
        }

        if (object->isAlive()) {
            moveObject(object, _field, deltaTimeMs);

            bool beforeMoveFlag = object->isMove();
            object->update();
            bool afterMoveFlag = object->isMove();

            // �������� ����� �������
            if (!beforeMoveFlag && afterMoveFlag) {
                Position forwardPos = object->getEndPosition();
                if (_field->isWall(forwardPos)) {
                    object->setMoveFlag(false);
                    object->setEndPosition(object->getBeginPosition());

                    if (object == _player) {
                        _player->clearPath();
                    }
                }
            }
        }

        // �������� �� ����� else, ��� ��� ������ ����� ������� � ������� ����
        if (!object->isAlive()) {
            _objects.erase(_objects.begin() + i);
            --i;
        }
    }

    updateLevelState();

    _lastUpdateTime = currentTime;
}

void Application::render() {
    uint32_t width = static_cast<uint32_t>(_field->getWidth());
    uint32_t height = static_cast<uint32_t>(_field->getHeight());
    PriorityTree priorityTree(width, height);

    addFieldToPriorityTree(priorityTree);
    addMarkersToPriorityTree(priorityTree);
    addObjectsToPriorityTree(priorityTree);

    _renderManager.setQueue(priorityTree.getSortedSprites());

    if (_state == LevelState::COMPLETED) {
        SpritePtr levelCompletedSprite = _resourceManager.getSprite("level_completed");
        renderInCenter(levelCompletedSprite);
    }
    else if (_state == LevelState::GAME_OVER) {
        SpritePtr gameOverSprite = _resourceManager.getSprite("game_over");
        renderInCenter(gameOverSprite);
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

void Application::renderInCenter(const SpritePtr sprite) {
    int w = 1024, h = 768;
    SDL_GetWindowSize(_window, &w, &h);

    SDL_Point cameraPoint = _camera->getPosition();
    SDL_Rect spriteRect = sprite->getRectangle();
    int x = (w - spriteRect.w) / 2 + cameraPoint.x;
    int y = (h - spriteRect.h) / 2 + cameraPoint.y;

    _renderManager.addToQueue(RenderData{ { x, y }, sprite });
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
    
    SDL_Point mouseGlobalPoint = _camera->getPosition();
    mouseGlobalPoint.x += event.motion.x;
    mouseGlobalPoint.y += event.motion.y;
    _currentPos = _field->getRowCol( convertToCoordinate(mouseGlobalPoint) );
}

void Application::handleMouseButton(const SDL_Event& event) noexcept {
    bool isButtonDown = (event.button.type == SDL_MOUSEBUTTONDOWN);

    if (event.button.button == SDL_BUTTON_MIDDLE) {
        _mouseControl = isButtonDown;
        return;
    }

    if (!isButtonDown || _state != LevelState::PLAYING) {
        return;
    }

    if (event.button.button == SDL_BUTTON_LEFT) {
        handleLeftMouseButton();
    }
    else if (event.button.button == SDL_BUTTON_RIGHT) {
        handleRightMouseButton();
    }
}

void Application::handleLeftMouseButton() noexcept {
    if (!_field->isCorrectPosition(_currentPos)) {
        return;
    }

    PathMatrix pathMatrix = generatePathMatrix(_field);
    fillDistance(pathMatrix, _player->getEndPosition(), _currentPos);
    
    deque<Direction> path = searchPath(pathMatrix, _currentPos);
    _player->setPath( move(path) );
}

void Application::handleRightMouseButton() noexcept {
    if (!_field->isCorrectPosition(_currentPos)) {
        return;
    }

    // E��� ������ �������� �����-���� ������, �� ��������� ����� ���������.
    // ���������� ��������� ��� � ������� _field, �� ����� ��������� ���� �������������� ����� � ��������� Tile.
    // � ������ ������ ������� �� ����� ����� �������, ���� ���������� �������� �� �����.
    for (IDynamicObjectPtr object : _objects) {
        if (_currentPos == object->getBeginPosition()
            || _currentPos == object->getEndPosition()) {
            return;
        }
    }

    if (_field->getState(_currentPos) == TileState::DEFAULT) {
        _field->setState(_currentPos, TileState::WALL_DEFAULT);
    }
    else if (_field->getState(_currentPos) == TileState::WALL_DEFAULT) {
        _field->setState(_currentPos, TileState::DEFAULT);
    }
}

void Application::generateField(uint32_t width, uint32_t height) {
    _field = make_shared<Field>(
        width,
        height,
        TILE_DESCRIPTION
    );

    _finishPos = { 1, 1 };
    _field->setState(_finishPos, TileState::FINISH);

    int w = static_cast<int>(width);
    int h = static_cast<int>(height);

    for (int column = 0; column < w; ++column) {
        _field->setState({ 0, column }, TileState::WALL_BORDER);
        _field->setState({ h - 1, column }, TileState::WALL_BORDER);
    }

    for (int row = 1; row < h - 1; ++row) {
        _field->setState({ row, 0 }, TileState::WALL_BORDER);
        _field->setState({ row, w - 1 }, TileState::WALL_BORDER);
    }
}

void Application::addFieldToPriorityTree(PriorityTree& tree) const {
    const TileMatrix& tiles = _field->getTiles();
    int width = _field->getWidth();
    int height = _field->getHeight();

    for (int i = 0; i < height; ++i) {
        const vector<Tile> row = tiles[i];

        for (int j = 0; j < width; ++j) {
            const Tile& tile = row[j];
            string spriteName = getTileName(tile.state);
            SpritePtr sprite = _resourceManager.getSprite(spriteName);

            if (sprite != nullptr) {
                tree.addSprite(
                    RenderData{ tile.drawPoint, sprite },
                    Position{ i, j }
                );
            }
        }
    }
}

void Application::addMarkersToPriorityTree(PriorityTree& tree) const {
    bool isCorrectPosition = _field->isCorrectPosition(_currentPos);

    if (isCorrectPosition && _field->isWalkable(_currentPos)) {
        SpritePtr cursorSprite = _resourceManager.getSprite("cursor");
        Coordinate addCoord = _field->getSpriteCoord(_currentPos);

        tree.addSprite(
            RenderData{ convertToSdlPoint(addCoord), cursorSprite },
            _currentPos
        );
    }

    if (_player->isMove()) {
        deque<Direction> path = _player->getPath();
        path.push_front( _player->getDirection() );

        SpritePtr pointSprite = _resourceManager.getSprite("point");
        Position currentPos = _player->getBeginPosition();

        for (Direction dir : path) {
            currentPos = nextPosition(currentPos, dir);
            
            Coordinate addCoord = _field->getSpriteCoord(currentPos);

            tree.addSprite(
                RenderData{ convertToSdlPoint(addCoord), pointSprite },
                currentPos
            );
        }
    }
}

void Application::addObjectsToPriorityTree(PriorityTree& tree) const {
    for (IDynamicObjectPtr object : _objects) {
        SpritePtr sprite = object->getAnimation()->getSprite();
        Coordinate isometricCoord = _field->getSpriteCoord( object->getCartesianCoord() );
        Position beginPos = object->getBeginPosition();
        Direction dir = object->getDirection();

        if (object->isMove()) {
            tree.addSprite(
                { convertToSdlPoint(isometricCoord), sprite },
                beginPos,
                dir
            );
        }
        else {
            tree.addSprite(
                { convertToSdlPoint(isometricCoord), sprite },
                beginPos
            );
        }
    }
}

void Application::updateLevelState() {
    if (_state == LevelState::PLAYING) {
        if (!_player->isAlive()) {
            _state = LevelState::GAME_OVER;
        }
        else if (!_player->isMove() && _player->getBeginPosition() == _finishPos) {
            _state = LevelState::COMPLETED;
        }
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
