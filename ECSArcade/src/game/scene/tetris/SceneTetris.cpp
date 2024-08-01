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
    auto shape = shapes[activeTetromino.shapeIndex];
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
    auto shape = shapes[activeTetromino.shapeIndex];
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
    auto shape = shapes[tetromino.shapeIndex];
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

        auto shape = shapes[tetromino->shapeIndex];
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

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->next) continue;

        auto xOffset = columns + 2;
        auto yOffset = 4;

        auto shape = shapes[tetromino->shapeIndex];
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