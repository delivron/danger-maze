#include <utility>
#include <iostream>
#include <algorithm>

#include "SDL_image.h"

#include "simulation.h"
#include "application.h"
#include "path_searching.h"

#include "../object/enemy.h"

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

const TileDescription Application::TILE_DESCRIPTION = {
    70, // spriteWidth
    81, // spriteHeight
    35, // halfHorizontalDiag
    20, // halfVerticalDiag
    35, // tileX
    60  // tileY
};

const SDL_Color Application::BACKGROUND_COLOR = { 155, 221, 255, 255 };

const uint32_t Application::FIELD_SIZE = 30;

const float Application::PLAYER_SPEED = 90.0f;

const float Application::ENEMY_SPEED = 125.0f;

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
        settings.displayWidth,
        settings.displayHeight,
        SDL_WINDOW_SHOWN
    );
    CHECK_SDL_RESULT(_window == nullptr, "SDL_CreateWindow");

    if (settings.fullscreen) {
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
    _resourceManager.loadAnimationFromDescription("sprites/enemy.xml", renderer);

    generateField(FIELD_SIZE, FIELD_SIZE);

    _camera = make_shared<Camera>(
        settings.displayWidth,
        settings.displayHeight,
        generateVisibleRect(_field, settings.displayWidth, settings.displayHeight)
    );

    initializeObjects();

    Position playerPos = _player->getBeginPosition();
    SDL_Point cameraPosition = convertToSdlPoint( _field->getIsometricCoord(playerPos) );
    cameraPosition.x -= settings.displayWidth / 2;
    cameraPosition.y -= settings.displayHeight / 2;
    _camera->setPosition(cameraPosition);

    return true;
}

void Application::update() {
    clock_t currentTime = clock();
    clock_t deltaTimeMs = currentTime - _lastUpdateTime;
    if (deltaTimeMs > MS_MAX_SIMULATE) {
        deltaTimeMs = MS_MAX_SIMULATE;
    }

    for (size_t i = 0; i < _objects.size(); ++i) {
        IDynamicObjectPtr object = _objects[i];
        
        if (object->isAlive()) {
            object->update();
            moveObject(object, _field, deltaTimeMs);
        }

        // написано не через else, так как объект может умереть в условии выше
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

    sort(_objects.begin(), _objects.end(), [](IDynamicObjectPtr lhs, IDynamicObjectPtr rhs) {
        Position leftPos = lhs->getBeginPosition();
        Position rightPos = rhs->getBeginPosition();
        return leftPos < rightPos;
    });
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

    // Eсли клетка задевает какой-либо объект, то установка стены запрещена.
    // Оптимально проверять это с помощью _field, но тогда требуется ввод дополнительных полей в структуру Tile.
    // В данном случае перебор не займёт много времени, если динамичных объектов не много.
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

void Application::initializeObjects() {
    Position playerPos = { _field->getHeight() - 2, _field->getWidth() - 2 };
    _player = make_shared<Player>(_resourceManager.getAnimation("player"), PLAYER_SPEED);
    _objects.push_back(_player);
    addObject(_field, _player, playerPos);

    addEnemy({ 1, 4 }, { 1, 4 }, { 28, 4 }, Direction::DOWN);
    addEnemy({ 28, 4 }, { 1, 4 }, { 28, 4 }, Direction::UP);

    addEnemy({ 1, 25 }, { 1, 25 }, { 28, 25 }, Direction::DOWN);
    addEnemy({ 28, 25 }, { 1, 25 }, { 28, 25 }, Direction::UP);

    addEnemy({ 4, 1 }, { 4, 1 }, { 4, 14 }, Direction::RIGHT);
    addEnemy({ 4, 28 }, { 4, 15 }, { 4, 28 }, Direction::LEFT);

    addEnemy({ 11, 11 }, { 11, 11 }, { 11, 18 }, Direction::RIGHT);
    addEnemy({ 11, 18 }, { 11, 18 }, { 18, 18 }, Direction::DOWN);
    addEnemy({ 18, 18 }, { 18, 11 }, { 18, 18 }, Direction::LEFT);
    addEnemy({ 18, 11 }, { 11, 11 }, { 18, 11 }, Direction::UP);

    addEnemy({ 25, 26 }, { 25, 26 }, { 25, 28 }, Direction::RIGHT);
    addEnemy({ 23, 28 }, { 23, 26 }, { 23, 28 }, Direction::LEFT);
    addEnemy({ 21, 26 }, { 21, 26 }, { 21, 28 }, Direction::RIGHT);
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

    if (_player->isAlive()) {
        deque<Direction> path = _player->getPath();
        if (_player->isMove()) {
            path.push_front(_player->getDirection());
        }

        SpritePtr pointSprite = _resourceManager.getSprite("point");
        Position currentPos = _player->getBeginPosition();

        for (Direction dir : path) {
            currentPos = nextPosition(currentPos, dir);
            
            if (!_field->isWalkable(currentPos)) {
                break;
            }

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

void Application::addEnemy(const Position& addPos, const Position& p1, const Position& p2, Direction direction) {
    EnemyPtr enemy = make_shared<Enemy>(_resourceManager.getAnimation("enemy"), ENEMY_SPEED);
    enemy->setControlZone(ControlZone{ p1, p2 });
    enemy->setDirection(direction);
    
    addObject(_field, enemy, addPos);
    _objects.push_back(enemy);
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
