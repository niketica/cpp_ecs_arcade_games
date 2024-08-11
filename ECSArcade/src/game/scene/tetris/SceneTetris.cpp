#include "SceneTetris.h"

SceneTetris::SceneTetris(Game* game) : Scene(game) {}

void SceneTetris::init()
{
    getGame()->getECSManager().reset();
    createActionList();
    createNextTetromino();
    activateNextTetromino();

    Entity eGameStatus = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<EntityTag>(eGameStatus, EntityTag(TETRIS_STATUS));
    getGame()->getECSManager().addComponent<TetrisStatus>(eGameStatus, {});
}

void SceneTetris::createNextTetromino()
{
    Tetromino tetromino = tetrominos[rand.getRandomNumber(nrOfTetrominos - 1)];
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
    trimFirstRow(tetromino);
    trimFirstCol(tetromino);

    EntityTag tag{ TETROMINO };

    auto entity = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<EntityTag>(entity, tag);
    getGame()->getECSManager().addComponent<Tetromino>(entity, tetromino);
}

void SceneTetris::activateNextTetromino()
{
    for (const auto& entity : getGame()->getEntitiesWithTag(TETROMINO))
    {
        auto tetromino = getGame()->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->next) continue;

        tetromino->next = false;
        tetromino->active = true;

        if (isCollisionBottom(*tetromino) && isCollisionHorizontal(*tetromino, 0))
        {
            for (const auto& eStatus : getGame()->getEntitiesWithTag(TETRIS_STATUS))
            {
                auto status = getGame()->getECSManager().getComponent<TetrisStatus>(eStatus);
                status->running = false;
            }
        }

        break;
    }

    createNextTetromino();
}

void SceneTetris::createActionList()
{
    Entity eAction = getGame()->getECSManager().addEntity();
    std::vector<TetrisAction> actionList;
    getGame()->getECSManager().addComponent<std::vector<TetrisAction>>(eAction, actionList);
}

void SceneTetris::input()
{
    auto keyInputs = getGame()->getECSManager().getComponents<KeyInput>();
    for (const auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<TetrisAction>>();
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
                std::shared_ptr<Scene> scene = std::make_shared<SceneTitleScreen>(getGame());
                getGame()->loadScene(scene);
                return;
            }
        }
    }
}

void SceneTetris::update(float deltaTime)
{
    for (const auto& entity : getGame()->getEntitiesWithTag(TETRIS_STATUS))
    {
        auto status = getGame()->getECSManager().getComponent<TetrisStatus>(entity);

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

    auto eTetrominos = getGame()->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = getGame()->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->active) continue;

        if (isCollisionBottom(*tetromino))
        {
            updateGrid(*tetromino);
            getGame()->getECSManager().removeEntity(entity);
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
    auto eTetrominos = getGame()->getEntitiesWithTag(TETROMINO);

    for (auto& entity : eTetrominos)
    {
        auto tetromino = getGame()->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->active) continue;

        auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<TetrisAction>>();
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

void SceneTetris::updateGrid(const Tetromino& tetromino)
{
    const auto& shape = tetromino.shape.value;
    for (int y = 0; y < shapeSize; y++)
    {
        for (int x = 0; x < shapeSize; x++)
        {
            if (shape[y][x] == 0) continue;

            auto xPos = x + tetromino.topLeftPos.x;
            auto yPos = y + tetromino.topLeftPos.y;
            Block block{ Vec2{xPos, yPos}, tetromino.color };

            auto eBlock = getGame()->getECSManager().addEntity();
            getGame()->getECSManager().addComponent<EntityTag>(eBlock, EntityTag { TETRIS_BLOCK });
            getGame()->getECSManager().addComponent<Block>(eBlock, block);
        }
    }
}

bool SceneTetris::isCollisionBottom(const Tetromino& activeTetromino)
{
    auto shape = activeTetromino.shape.value;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (shape[y][x] == 0) continue;

            auto xPos = x + activeTetromino.topLeftPos.x;
            auto yPos = y + activeTetromino.topLeftPos.y;

            if (yPos + 1 >= rows || blockOccupiesPosition(Vec2{ xPos, yPos + 1 }))
            {
                return true;
            }
        }
    }

    return false;
}

bool SceneTetris::isCollisionHorizontal(const Tetromino& activeTetromino, const int xOffset)
{
    auto shape = activeTetromino.shape.value;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (shape[y][x] == 0) continue;

            auto xPos = x + activeTetromino.topLeftPos.x;
            auto yPos = y + activeTetromino.topLeftPos.y;

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
    auto eBlocks = getGame()->getEntitiesWithTag(TETRIS_BLOCK);
    for (auto& entity : eBlocks)
    {
        auto block = getGame()->getECSManager().getComponent<Block>(entity);

        if (block->position.x == pos.x && block->position.y == pos.y)
        {
            return true;
        }
    }

    return false;
}

void SceneTetris::rotate(Tetromino& tetromino)
{
    auto& currentShape = tetromino.shape.value;

    std::array<std::array<int, shapeSize>, shapeSize> oldShape{ {} };
    std::array<std::array<int, shapeSize>, shapeSize> newShape{ {} };

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

    trimFirstRow(tetromino);
    trimFirstCol(tetromino);

    bool collision = false;
    for (int x = 0; x < shapeSize; x++)
    {
        for (int y = 0; y < shapeSize; y++)
        {
            if (currentShape[y][x] == 0) continue;

            auto xPos = x + tetromino.topLeftPos.x;
            auto yPos = y + tetromino.topLeftPos.y;

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

void SceneTetris::trimFirstRow(Tetromino& tetromino)
{
    auto& currentShape = tetromino.shape.value;

    for (int x = 0; x < shapeSize; x++)
    {
        if (currentShape[0][x] > 0)
        {
            return;
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

    trimFirstRow(tetromino);
    return;
}

void SceneTetris::trimFirstCol(Tetromino& tetromino)
{
    auto& currentShape = tetromino.shape.value;

    for (int y = 0; y < shapeSize; y++)
    {
        if (currentShape[y][0] > 0)
        {
            return;
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

    trimFirstCol(tetromino);
    return;
}

void SceneTetris::processClearRow()
{
    auto eStatus = getGame()->getEntitiesWithTag(TETRIS_STATUS);

    for (int y = 0; y < rows; y++)
    {
        while (isRowFilled(y))
        {
            clearRow(y);

            for (auto& entity : eStatus)
            {
                auto status = getGame()->getECSManager().getComponent<TetrisStatus>(entity);
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

    auto eBlocks = getGame()->getEntitiesWithTag(TETRIS_BLOCK);
    for (auto& entity : eBlocks)
    {
        auto block = getGame()->getECSManager().getComponent<Block>(entity);

        if (block->position.y == y)
        {
            rowValue++;
        }
    }

    return rowValue == columns;
}

void SceneTetris::clearRow(int y)
{
    for (auto& entity : getGame()->getEntitiesWithTag(TETRIS_BLOCK))
    {
        auto block = getGame()->getECSManager().getComponent<Block>(entity);

        if (block->position.y == y)
        {
            getGame()->getECSManager().removeEntity(entity);
        }
    }

    for (auto& entity : getGame()->getEntitiesWithTag(TETRIS_BLOCK))
    {
        auto block = getGame()->getECSManager().getComponent<Block>(entity);

        if (block->position.y < y)
        {
            block->position.y++;
        }
    }
}

void SceneTetris::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    renderGrid(window);
    renderNextTetromino(window);
    renderScore(window);

    window.display();
}

void SceneTetris::setBlockColor(const GridBlock& blok) const
{
    sf::VertexArray& vArray = blok.vArray;
    const int width = blok.width;
    const int x = blok.x;
    const int y = blok.y;
    const int xOffset = blok.xOffset;
    const int yOffset = blok.yOffset;
    const int r = blok.r;
    const int g = blok.g;
    const int b = blok.b;

    // get a pointer to the triangles' vertices of the current tile
    sf::Vertex* triangles = &vArray[(x + y * width) * 6];

    // define the 6 corners of the two triangles
    triangles[0].position = sf::Vector2f(x * cellSize + xOffset, y * cellSize + yOffset);
    triangles[1].position = sf::Vector2f((x + 1) * cellSize + xOffset, y * cellSize + yOffset);
    triangles[2].position = sf::Vector2f(x * cellSize + xOffset, (y + 1) * cellSize + yOffset);
    triangles[3].position = sf::Vector2f(x * cellSize + xOffset, (y + 1) * cellSize + yOffset);
    triangles[4].position = sf::Vector2f((x + 1) * cellSize + xOffset, y * cellSize + yOffset);
    triangles[5].position = sf::Vector2f((x + 1) * cellSize + xOffset, (y + 1) * cellSize + yOffset);

    sf::Color color(r,g,b);

    triangles[0].color = color;
    triangles[1].color = color;
    triangles[2].color = color;
    triangles[3].color = color;
    triangles[4].color = color;
    triangles[5].color = color;
}

void SceneTetris::renderGridLines(sf::RenderWindow& window)
{
    auto width = getGame()->getWidth();
    auto height = getGame()->getHeight();

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

void SceneTetris::renderGrid(sf::RenderWindow& window)
{
    int vertexArraySize = columns * rows * 6;
    sf::VertexArray vArray(sf::PrimitiveType::Triangles, vertexArraySize);
    for (int y = 0; y < rows; y++)
    {
        for (int x = 0; x < columns; x++)
        {
            setBlockColor({ vArray, columns, x, y, 0, 0, 0, 0, 0 });
        }
    }

    auto eBlocks = getGame()->getEntitiesWithTag(TETRIS_BLOCK);
    for (const auto& entity : eBlocks)
    {
        auto block = getGame()->getECSManager().getComponent<Block>(entity);
        const auto position = block->position;
        const auto x = (int)position.x;
        const auto y = (int)position.y;
        const sf::Color& color = block->color;
        setBlockColor({ vArray, columns, x, y, 0, 0, color.r, color.g, color.b });
    }

    auto eTetrominos = getGame()->getEntitiesWithTag(TETROMINO);
    for (const auto& entity : eTetrominos)
    {
        auto tetromino = getGame()->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->active) continue;

        const auto shape = tetromino->shape.value;
        const sf::Color color = tetromino->color;
        const auto position = tetromino->topLeftPos;
        const auto xOffset = (int)position.x;
        const auto yOffset = (int)position.y;

        for (int x = 0; x < shapeSize; x++)
        {
            for (int y = 0; y < shapeSize; y++)
            {
                if (shape[y][x] == 0) continue;
                const int xPos = x + xOffset;
                const int yPos = y + yOffset;
                setBlockColor({ vArray, columns, xPos, yPos, 0, 0, color.r, color.g, color.b });
            }
        }
    }
    window.draw(vArray);
    renderGridLines(window);
}

void SceneTetris::renderNextTetromino(sf::RenderWindow& window)
{
    const int xOffset = (columns + 2) * cellSize;
    const int yOffset = 4 * cellSize;

    int vertexArraySize = shapeSize * shapeSize * 6;
    sf::VertexArray vArray(sf::PrimitiveType::Triangles, vertexArraySize);
    for (int y = 0; y < shapeSize; y++)
    {
        for (int x = 0; x < shapeSize; x++)
        {
            setBlockColor({ vArray, shapeSize, x, y, xOffset, yOffset, 0, 0, 0 });
        }
    }

    auto eTetrominos = getGame()->getEntitiesWithTag(TETROMINO);
    for (const auto& entity : eTetrominos)
    {
        auto tetromino = getGame()->getECSManager().getComponent<Tetromino>(entity);

        if (!tetromino->next) continue;

        auto shape = tetromino->shape.value;
        for (int x = 0; x < shapeSize; x++)
        {
            for (int y = 0; y < shapeSize; y++)
            {
                if (shape[y][x] == 0) continue;

                sf::Color color = tetromino->color;
                setBlockColor({ vArray, shapeSize, x, y, xOffset, yOffset, color.r, color.g, color.b });
            }
        }
    }

    window.draw(vArray);
}

void SceneTetris::renderScore(sf::RenderWindow& window)
{
    auto eStatus = getGame()->getEntitiesWithTag(TETRIS_STATUS);
    for (const auto& entity : eStatus)
    {
        auto status = getGame()->getECSManager().getComponent<TetrisStatus>(entity);
        auto& openSans = getGame()->getAssetManager().getFont("OpenSans");
        sf::Text scoreTxt("Score: " + std::to_string(status->rowsCleared), openSans, 34);

        auto xPos = (columns + 1) * cellSize;
        auto yPos = (rows * cellSize) / 2;
        scoreTxt.setPosition(xPos, yPos);
        scoreTxt.setFillColor(sf::Color(255, 255, 255));
        window.draw(scoreTxt);
    }
}