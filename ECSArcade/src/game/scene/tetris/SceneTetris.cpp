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

    Entity eGameStatus = game->getECSManager().addEntity();
    game->getECSManager().addComponent<EntityTag>(eGameStatus, TETRIS_STATUS);
    game->getECSManager().addComponent<TetrisStatus>(eGameStatus, {});
}

void SceneTetris::createNextTetromino()
{
    Tetromino tetromino = tetrominos[rand() % nrOfTetrominos];
    tetromino.topLeftPos = { (columns / 2) - ( shapeSize / 2), 0 };
    tetromino.active = false;
    tetromino.next = true;

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

        if (isCollisionBottom(*tetromino) && isCollisionHorizontal(*tetromino, 0))
        {
            auto eStatus = game->getEntitiesWithTag(TETRIS_STATUS);
            for (auto& entity : eStatus)
            {
                auto status = game->getECSManager().getComponent<TetrisStatus>(entity);
                status->running = false;
            }
        }

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
            else if (keyInput->keyType == R)
            {
                init();
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

void SceneTetris::update(float deltaTime)
{
    auto eStatus = game->getEntitiesWithTag(TETRIS_STATUS);
    for (auto& entity : eStatus)
    {
        auto status = game->getECSManager().getComponent<TetrisStatus>(entity);

        if (!status->running)
        {
            return;
        }

        processActions();

        if (status->currentMovementCooldown > 0)
        {
            status->currentMovementCooldown--;
            return;
        }
        status->currentMovementCooldown = status->movementCooldown;
    }

    auto eTetrominos = game->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->active) continue;

        if (isCollisionBottom(*tetromino))
        {
            updateGrid(*tetromino);
            game->getECSManager().removeEntity(entity);
            processClearRow();
            activateNextTetromino();
            return;
        }

        tetromino->topLeftPos.y++;
        break;
    }
}

void SceneTetris::processActions()
{
    auto eTetrominos = game->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = game->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->active) continue;

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
        break;
    }
}

void SceneTetris::updateGrid(Tetromino& tetromino)
{
    auto& shape = tetromino.shape.value;
    for (int y = 0; y < shapeSize; y++)
    {
        for (int x = 0; x < shapeSize; x++)
        {
            if (shape[y][x] == 0) continue;

            float xPos = x + tetromino.topLeftPos.x;
            float yPos = y + tetromino.topLeftPos.y;

            Block block{ Vec2{xPos, yPos}, tetromino.color };
            auto eBlock = game->getECSManager().addEntity();
            game->getECSManager().addComponent<EntityTag>(eBlock, EntityTag { TETRIS_BLOCK });
            game->getECSManager().addComponent<Block>(eBlock, block);
        }
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

            if (yPos + 1 >= rows || blockOccupiesPosition(Vec2{ xPos, yPos + 1 }))
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

            if (collideWallLeft || collideWallRight || blockOccupiesPosition(Vec2{ xPos + xOffset, yPos }))
            {
                return true;
            }
        }
    }

    return false;
}

bool SceneTetris::blockOccupiesPosition(Vec2 pos)
{
    auto eBlocks = game->getEntitiesWithTag(TETRIS_BLOCK);
    for (auto& entity : eBlocks)
    {
        auto block = game->getECSManager().getComponent<Block>(entity);

        if (block->position.x == pos.x && block->position.y == pos.y)
        {
            return true;
        }
    }

    return false;
}

void SceneTetris::rotate(Tetromino& tetromino)
{
    // TODO check if rotation does not collide with other blocks / edge of screen

    auto& currentShape = tetromino.shape.value;
    int oldShape[shapeSize][shapeSize] = {};
    int newShape[shapeSize][shapeSize] = {};

    int newX = shapeSize - 1;

    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            oldShape[y][x] = currentShape[y][x];
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

    bool collision = false;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (currentShape[y][x] == 0) continue;

            float xPos = x + tetromino.topLeftPos.x;
            float yPos = y + tetromino.topLeftPos.y;

            if (xPos < 0 || xPos >= columns || yPos < 0 || yPos >= rows || blockOccupiesPosition( Vec2{ xPos, yPos } ))
            {
                collision = true;
                break;
            }
        }
    }

    if (collision)
    {
        for (int x = 0; x < shapeSize; x++)
        {
            for (int y = 0; y < shapeSize; y++)
            {
                currentShape[y][x] = oldShape[y][x];
            }
        }
    }
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

void SceneTetris::processClearRow()
{
    auto eStatus = game->getEntitiesWithTag(TETRIS_STATUS);

    for (int y = 0; y < rows; y++)
    {
        while (isRowFilled(y))
        {
            clearRow(y);

            for (auto& entity : eStatus)
            {
                auto status = game->getECSManager().getComponent<TetrisStatus>(entity);
                status->rowsCleared++;
                status->movementCooldown = status->startingCooldown - status->rowsCleared;
                if (status->movementCooldown < 2) status->movementCooldown = 2;
            }
        }
    }
}

bool SceneTetris::isRowFilled(int y)
{
    int rowValue = 0;

    auto eBlocks = game->getEntitiesWithTag(TETRIS_BLOCK);
    for (auto& entity : eBlocks)
    {
        auto block = game->getECSManager().getComponent<Block>(entity);

        if (block->position.y == y)
        {
            rowValue++;
        }
    }

    return rowValue == columns;
}

void SceneTetris::clearRow(int y)
{
    for (auto& entity : game->getEntitiesWithTag(TETRIS_BLOCK))
    {
        auto block = game->getECSManager().getComponent<Block>(entity);

        if (block->position.y == y)
        {
            game->getECSManager().removeEntity(entity);
        }
    }

    for (auto& entity : game->getEntitiesWithTag(TETRIS_BLOCK))
    {
        auto block = game->getECSManager().getComponent<Block>(entity);

        if (block->position.y < y)
        {
            block->position.y++;
        }
    }
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

        if (!tetromino->active) continue;

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

    auto eBlocks = game->getEntitiesWithTag(TETRIS_BLOCK);
    for (auto& entity : eBlocks)
    {
        auto block = game->getECSManager().getComponent<Block>(entity);

        sf::RectangleShape blockRect(sf::Vector2f(cellSize, cellSize));
        blockRect.setFillColor(block->color);
        blockRect.setPosition(block->position.x * cellSize, block->position.y * cellSize);
        window.draw(blockRect);
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

    auto eStatus = game->getEntitiesWithTag(TETRIS_STATUS);
    for (auto& entity : eStatus)
    {
        auto status = game->getECSManager().getComponent<TetrisStatus>(entity);
        auto& openSans = game->getAssetManager().getFont("OpenSans");
        sf::Text scoreTxt("Score: " + std::to_string(status->rowsCleared), openSans, 34);

        auto xPos = (columns + 1) * cellSize;
        auto yPos = (rows * cellSize) / 2;
        scoreTxt.setPosition(xPos, yPos);
        scoreTxt.setFillColor(sf::Color(255, 255, 255));
        window.draw(scoreTxt);
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