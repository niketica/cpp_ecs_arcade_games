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
            if (keyInput->keyType == W)
            {
                std::cout << "W pressed" << std::endl;
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