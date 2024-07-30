#include "SceneSnake.h"

SceneSnake::SceneSnake(Game* game) : Scene(game)
{
    init();
}

void SceneSnake::init()
{
    srand(time(NULL));

    createActionList();
    createPlayer();
    createPickup();
}

void SceneSnake::createActionList()
{
    Entity eAction = game->getECSManager().addEntity();
    std::vector<SnakeAction> actionList;
    game->getECSManager().addComponent<std::vector<SnakeAction>>(eAction, actionList);
}

void SceneSnake::createPlayer()
{
    auto eSnake = game->getECSManager().addEntity();
    game->getECSManager().addComponent<EntityTag>(eSnake, EntityTag(SNAKE_PLAYER));
    game->getECSManager().addComponent<SnakePlayer>(eSnake, SnakePlayer());
}

void SceneSnake::createPickup()
{
    auto eSnake = game->getEntityWithTag(SNAKE_PLAYER);
    auto player = game->getECSManager().getComponent<SnakePlayer>(eSnake);

    auto& snakeHead = player->head;
    auto& snakeBody = player->body;

    bool posFound = false;

    Vec2 pos = getRandomPosition();

    while (!posFound)
    {
        if (snakeHead == pos || std::find(snakeBody.begin(), snakeBody.end(), pos) != snakeBody.end())
        {
            pos = getRandomPosition();
        }
        else
        {
            posFound = true;
        }
    }

    auto ePickup = game->getECSManager().addEntity();
    game->getECSManager().addComponent<EntityTag>(ePickup, EntityTag(SNAKE_PICKUP));
    game->getECSManager().addComponent<SnakePickup>(ePickup, SnakePickup(pos));
}

Vec2 SceneSnake::getRandomPosition()
{
    return Vec2(rand() % columns, rand() % rows);
}

void SceneSnake::input()
{
    auto eSnake = game->getEntityWithTag(SNAKE_PLAYER);
    auto player = game->getECSManager().getComponent<SnakePlayer>(eSnake);

    auto keyInputs = game->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = game->getECSManager().getAnyComponent<std::vector<SnakeAction>>();
            if (keyInput->keyType == W)
            {
                if (player->currentAction != SNAKE_DOWN)
                {
                    player->nextAction = SNAKE_UP;
                }
            }
            else if (keyInput->keyType == S)
            {
                if (player->currentAction != SNAKE_UP)
                {
                    player->nextAction = SNAKE_DOWN;
                }
            }
            else if (keyInput->keyType == A)
            {
                if (player->currentAction != SNAKE_RIGHT)
                {
                    player->nextAction = SNAKE_LEFT;
                }
            }
            else if (keyInput->keyType == D)
            {
                if (player->currentAction != SNAKE_LEFT)
                {
                    player->nextAction = SNAKE_RIGHT;
                }
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

void SceneSnake::update()
{
    if (currentMovementCooldown > 0)
    {
        currentMovementCooldown--;
        return;
    }
    currentMovementCooldown = movementCooldown;

    auto eSnake = game->getEntityWithTag(SNAKE_PLAYER);
    auto player = game->getECSManager().getComponent<SnakePlayer>(eSnake);

    player->currentAction = player->nextAction;
    switch (player->currentAction)
    {
    case SNAKE_UP:
        moveSnakeUp(*player);
        break;
    case SNAKE_DOWN:
        moveSnakeDown(*player);
        break;
    case SNAKE_LEFT:
        moveSnakeLeft(*player);
        break;
    case SNAKE_RIGHT:
        moveSnakeRight(*player);
        break;
    default:
        std::cerr << "Undefined action." << std::endl;
        break;
    }

    checkPickup();
}

void SceneSnake::moveSnakeUp(SnakePlayer& player)
{
    if (player.head.y > 0)
    {
        player.head.y--;
    }
}

void SceneSnake::moveSnakeDown(SnakePlayer& player)
{
    if (player.head.y < rows - 1)
    {
        player.head.y++;
    }
}

void SceneSnake::moveSnakeLeft(SnakePlayer& player)
{
    if (player.head.x > 0)
    {
        player.head.x--;
    }
}

void SceneSnake::moveSnakeRight(SnakePlayer& player)
{
    if (player.head.x < columns - 1)
    {
        player.head.x++;
    }
}

void SceneSnake::checkPickup()
{
    auto eSnake = game->getEntityWithTag(SNAKE_PLAYER);
    auto player = game->getECSManager().getComponent<SnakePlayer>(eSnake);
    auto ePickup = game->getEntityWithTag(SNAKE_PICKUP);
    auto pickup = game->getECSManager().getComponent<SnakePickup>(ePickup);

    if (player->head.x == pickup->position.x &&
        player->head.y == pickup->position.y)
    {
        game->getECSManager().removeEntity(ePickup);
        createPickup();
    }
}

void SceneSnake::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    renderPickup(window);
    renderPlayer(window);
    renderGrid(window);

    window.display();
}

void SceneSnake::renderGrid(sf::RenderWindow& window)
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

void SceneSnake::renderPlayer(sf::RenderWindow& window)
{
    auto eSnake = game->getEntityWithTag(SNAKE_PLAYER);
    auto player = game->getECSManager().getComponent<SnakePlayer>(eSnake);
    auto head = player->head;

    sf::RectangleShape snakeHead(sf::Vector2f(cellSize, cellSize));
    snakeHead.setFillColor(sf::Color(0, 255, 0));
    snakeHead.setPosition(head.x * cellSize, head.y * cellSize);
    window.draw(snakeHead);
}

void SceneSnake::renderPickup(sf::RenderWindow& window)
{
    auto ePickup = game->getEntityWithTag(SNAKE_PICKUP);
    auto pickup = game->getECSManager().getComponent<SnakePickup>(ePickup);

    sf::RectangleShape snakeHead(sf::Vector2f(cellSize, cellSize));
    snakeHead.setFillColor(sf::Color(255, 255, 255));
    snakeHead.setPosition(pickup->position.x * cellSize, pickup->position.y * cellSize);
    window.draw(snakeHead);
}