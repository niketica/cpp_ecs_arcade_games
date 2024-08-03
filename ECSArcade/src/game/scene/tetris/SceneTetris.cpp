#include "SceneTetris.h"

SceneTetris::SceneTetris(Game* game) : Scene(game)
{
    init();
}

void SceneTetris::init()
{
    game->getECSManager().reset();
    srand(time(NULL));
    createActionList();
    createNextTetromino();
    activateNextTetromino();
}

void SceneTetris::createNextTetromino()
{
    Tetromino tetromino = tetrominos[rand() % nrOfTetrominos];
    tetromino.topLeftPos = { (columns / 2) - ( shapeSize / 2), 0 };
    tetromino.active = false;
    tetromino.next = true;

    if (tetromino.shapeIndex == 2) tetromino.topLeftPos.y--;

    auto& shape = shapes[tetromino.shapeIndex];
    Shape tShape = {};

    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            tShape.value[y][x] = shape[y][x];
        }
    }

    tetromino.shape = tShape;
    do {} while (trimFirstRow(tetromino));
    do {} while (trimFirstCol(tetromino));

    EntityTag tag{ TETROMINO };

    auto entity = game->getECSManager().addEntity();
    game->getECSManager().addComponent<EntityTag>(entity, tag);
    game->getECSManager().addComponent<Tetromino>(entity, tetromino);
}

void SceneTetris::activateNextTetromino()
{
    auto eTetrominos = game->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->next) continue;

        tetromino->next = false;
        tetromino->active = true;
        break;
    }

    createNextTetromino();
}

void SceneTetris::createActionList()
{
    Entity eAction = game->getECSManager().addEntity();
    std::vector<TetrisAction> actionList;
    game->getECSManager().addComponent<std::vector<TetrisAction>>(eAction, actionList);
}

void SceneTetris::input()
{
    auto keyInputs = game->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = game->getECSManager().getAnyComponent<std::vector<TetrisAction>>();
            if (keyInput->keyType == A)
            {
                actionList->push_back(TETRIS_LEFT);
            }
            else if (keyInput->keyType == D)
            {
                actionList->push_back(TETRIS_RIGHT);
            }
            else if (keyInput->keyType == S)
            {
                actionList->push_back(TETRIS_DOWN);
            }
            else if (keyInput->keyType == SPACE_KEY)
            {
                actionList->push_back(TETRIS_ROTATE);
            }
            else if (keyInput->keyType == ESCAPE_KEY)
            {
                std::shared_ptr<Scene> scene = std::make_shared<SceneTitleScreen>(game);
                game->loadScene(scene);
                return;
            }
        }
        else if (keyInput->inputType == RELEASED)
        {
        }
    }
}

void SceneTetris::update()
{
    if (currentMovementCooldown > 0)
    {
        currentMovementCooldown--;
        return;
    }
    currentMovementCooldown = movementCooldown;

    auto eTetrominos = game->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->active) continue;

        // checkCollisionBottom(*tetromino);

        auto actionList = game->getECSManager().getAnyComponent<std::vector<TetrisAction>>();
        for (auto& action : *actionList)
        {
            if (action == TETRIS_LEFT)
            {
                if (!isCollisionHorizontal(*tetromino, -1))
                {
                    tetromino->topLeftPos.x--;
                }
            }
            else if (action == TETRIS_RIGHT)
            {
                if (!isCollisionHorizontal(*tetromino, 1))
                {
                    tetromino->topLeftPos.x++;
                }
            }
            else if (action == TETRIS_DOWN)
            {
                if (!isCollisionBottom(*tetromino))
                {
                    tetromino->topLeftPos.y++;
                }
            }
            else if (action == TETRIS_ROTATE)
            {
                rotate(*tetromino);
            }
        }
        actionList->clear();

        if (isCollisionBottom(*tetromino))
        {
            tetromino->active = false;
            activateNextTetromino();
            return;
        }

        tetromino->topLeftPos.y++;
        break;
    }
}

bool SceneTetris::isCollisionBottom(Tetromino& activeTetromino)
{
    auto shape = activeTetromino.shape.value;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (shape[y][x] == 0) continue;

            float xPos = x + activeTetromino.topLeftPos.x;
            float yPos = y + activeTetromino.topLeftPos.y;

            if (yPos + 1 >= rows || tetrominoOccupiesPosition(Vec2{ xPos, yPos + 1 }))
            {
                return true;
            }
        }
    }

    return false;
}

bool SceneTetris::isCollisionHorizontal(Tetromino& activeTetromino, int xOffset)
{
    auto shape = activeTetromino.shape.value;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (shape[y][x] == 0) continue;

            float xPos = x + activeTetromino.topLeftPos.x;
            float yPos = y + activeTetromino.topLeftPos.y;

            bool collideWallLeft = false;
            bool collideWallRight = false;
            if (xOffset > 0)
            {
                collideWallRight = xPos >= columns - 1;
            }
            else
            {
                collideWallLeft = xPos <= 0;
            }

            if (collideWallLeft || collideWallRight || tetrominoOccupiesPosition(Vec2{ xPos + xOffset, yPos }))
            {
                return true;
            }
        }
    }

    return false;
}

bool SceneTetris::tetrominoOccupiesPosition(Vec2 pos)
{
    auto eTetrominos = game->getEntitiesWithTag(TETROMINO);
    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);
        if (tetromino->active || tetromino->next) continue;

        if (tetrominoOccupiesPosition(*tetromino, pos))
        {
            return true;
        }
    }
    return false;
}

bool SceneTetris::tetrominoOccupiesPosition(Tetromino& tetromino, Vec2 pos)
{
    auto shape = tetromino.shape.value;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (shape[y][x] == 0) continue;

            int xPos = x + tetromino.topLeftPos.x;
            int yPos = y + tetromino.topLeftPos.y;

            if (xPos == (int) pos.x && yPos == (int) pos.y)
            {
                return true;
            }
        }
    }
    return false;
}

void SceneTetris::rotate(Tetromino& tetromino)
{
    // TODO check if rotation does not collide with other blocks / edge of screen

    auto& currentShape = tetromino.shape.value;
    int newShape[shapeSize][shapeSize] = {};

    int newX = shapeSize - 1;

    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            newShape[x][newX - y] = currentShape[y][x];
        }
    }

    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            currentShape[y][x] = newShape[y][x];
        }
    }

    do {} while (trimFirstRow(tetromino));
    do {} while (trimFirstCol(tetromino));
}

bool SceneTetris::trimFirstRow(Tetromino& tetromino)
{
    auto& currentShape = tetromino.shape.value;

    for (int x = 0; x < shapeSize; x++)
    {
        if (currentShape[0][x] > 0)
        {
            return false;
        }
    }

    for (int y = 0; y < shapeSize - 1; y++)
    {
        for (int x = 0; x < shapeSize; x++)
        {
            currentShape[y][x] = currentShape[y + 1][x];
            currentShape[shapeSize - 1][x] = 0;
        }
    }

    return true;
}

bool SceneTetris::trimFirstCol(Tetromino& tetromino)
{
    auto& currentShape = tetromino.shape.value;

    for (int y = 0; y < shapeSize; y++)
    {
        if (currentShape[y][0] > 0)
        {
            return false;
        }
    }

    for (int y = 0; y < shapeSize; y++)
    {
        for (int x = 0; x < shapeSize - 1; x++)
        {
            currentShape[y][x] = currentShape[y][x + 1];
        }
        currentShape[y][shapeSize - 1] = 0;
    }

    return true;
}

void SceneTetris::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    renderBlocks(window);
    renderGrid(window);

    window.display();
}

void SceneTetris::renderBlocks(sf::RenderWindow& window)
{
    auto eTetrominos = game->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (tetromino->next) continue;

        auto xOffset = tetromino->topLeftPos.x;
        auto yOffset = tetromino->topLeftPos.y;

        auto shape = tetromino->shape.value;
        for (int x = 0; x < shapeSize; x++)
        {
            for (int y = 0; y < shapeSize; y++)
            {
                sf::Color color = tetromino->color;
                // if (shape[y][x] == 0) color = {255,255,255};
                if (shape[y][x] == 0) continue;

                sf::RectangleShape block(sf::Vector2f(cellSize, cellSize));
                block.setFillColor(color);
                block.setPosition((x + xOffset) * cellSize, (y + yOffset) * cellSize);
                window.draw(block);
            }
        }
    }

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->next) continue;

        auto xOffset = columns + 2;
        auto yOffset = 4;

        auto shape = tetromino->shape.value;
        for (int x = 0; x < shapeSize; x++)
        {
            for (int y = 0; y < shapeSize; y++)
            {
                if (shape[y][x] == 0) continue;

                sf::RectangleShape block(sf::Vector2f(cellSize, cellSize));
                block.setFillColor(tetromino->color);
                block.setPosition((x + xOffset) * cellSize, (y + yOffset) * cellSize);
                window.draw(block);
            }
        }
    }
}

void SceneTetris::renderGrid(sf::RenderWindow& window)
{
    auto width = game->getWidth();
    auto height = game->getHeight();

    for (int x = 0; x < cellSize * columns + cellSize; x += cellSize)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(x, 0)),
            sf::Vertex(sf::Vector2f(x, cellSize * rows))
        };

        window.draw(line, 2, sf::Lines);
    }

    for (int y = 0; y < cellSize * rows + cellSize; y += cellSize)
    {
        sf::Vertex line[] =
        {
            sf::Vertex(sf::Vector2f(0, y)),
            sf::Vertex(sf::Vector2f(cellSize * columns, y))
        };

        window.draw(line, 2, sf::Lines);
    }
}