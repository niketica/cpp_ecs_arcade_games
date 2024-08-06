#include "SceneSnake.h"

SceneSnake::SceneSnake(Game* game) : Scene(game)
{
    init();
}

void SceneSnake::init()
{
    getGame()->getECSManager().reset();

    srand(time(NULL));

    createActionList();
    createPlayer();
    createPickup();
}

void SceneSnake::createActionList()
{
    Entity eAction = getGame()->getECSManager().addEntity();
    std::vector<SnakeAction> actionList;
    getGame()->getECSManager().addComponent<std::vector<SnakeAction>>(eAction, actionList);
}

void SceneSnake::createPlayer()
{
    auto snake = SnakePlayer();
    snake.body.push_back(Vec2(2, 2));

    auto eSnake = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<EntityTag>(eSnake, EntityTag(SNAKE_PLAYER));
    getGame()->getECSManager().addComponent<SnakePlayer>(eSnake, snake);
}

void SceneSnake::createPickup()
{
    auto eSnake = getGame()->getEntityWithTag(SNAKE_PLAYER);
    auto player = getGame()->getECSManager().getComponent<SnakePlayer>(eSnake);

    auto& snakeBody = player->body;

    bool posFound = false;

    Vec2 pos = getRandomPosition();

    int maxPositions = columns * rows;
    int currentCount = 0;

    while (!posFound)
    {
        currentCount++;
        if (std::find(snakeBody.begin(), snakeBody.end(), pos) != snakeBody.end())
        {
            if (currentCount >= maxPositions)
            {
                // Player wins;
                init();
                return;
            }
            else
            {
                pos = getRandomPosition();
            }
        }
        else
        {
            posFound = true;
        }
    }

    auto ePickup = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<EntityTag>(ePickup, EntityTag(SNAKE_PICKUP));
    getGame()->getECSManager().addComponent<SnakePickup>(ePickup, SnakePickup(pos));
}

Vec2 SceneSnake::getRandomPosition()
{
    return Vec2(rand() % columns, rand() % rows);
}

void SceneSnake::input()
{
    auto eSnake = getGame()->getEntityWithTag(SNAKE_PLAYER);
    auto player = getGame()->getECSManager().getComponent<SnakePlayer>(eSnake);

    auto keyInputs = getGame()->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<SnakeAction>>();
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
                std::shared_ptr<Scene> scene = std::make_shared<SceneTitleScreen>(getGame());
                getGame()->loadScene(scene);
                return;
            }
        }
        else if (keyInput->inputType == RELEASED)
        {
        }
    }
}

void SceneSnake::update(float deltaTime)
{
    if (currentMovementCooldown > 0)
    {
        currentMovementCooldown--;
        return;
    }
    currentMovementCooldown = movementCooldown;

    auto eSnake = getGame()->getEntityWithTag(SNAKE_PLAYER);
    auto player = getGame()->getECSManager().getComponent<SnakePlayer>(eSnake);

    if (player->dead)
    {
        if (player->body.size() > 0)
        {
            player->body.pop_back();
        }
        else
        {
            init();
        }
        return;
    }

    updateBody(*player);

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

    checkSnakeCollision(*player);
    checkPickup();
}

void SceneSnake::moveSnakeUp(SnakePlayer& player)
{
    auto& head = player.body.front();
    if (head.y > 0)
    {
        head.y--;
    }
}

void SceneSnake::moveSnakeDown(SnakePlayer& player)
{
    auto& head = player.body.front();
    if (head.y < rows - 1)
    {
        head.y++;
    }
}

void SceneSnake::moveSnakeLeft(SnakePlayer& player)
{
    auto& head = player.body.front();
    if (head.x > 0)
    {
        head.x--;
    }
}

void SceneSnake::moveSnakeRight(SnakePlayer& player)
{
    auto& head = player.body.front();
    if (head.x < columns - 1)
    {
        head.x++;
    }
}

void SceneSnake::updateBody(SnakePlayer& player)
{
    auto& body = player.body;
    for (int i = body.size() - 1; i > 0; i--)
    {
        body.at(i) = body.at(i - 1);
    }
}

void SceneSnake::checkSnakeCollision(SnakePlayer& player)
{
    auto& body = player.body;
    auto head = body.front();

    for (int i = 1; i < body.size(); i++)
    {
        if (head == body.at(i))
        {
            player.dead = true;
            return;
        }
    }
}

void SceneSnake::checkPickup()
{
    auto eSnake = getGame()->getEntityWithTag(SNAKE_PLAYER);
    auto player = getGame()->getECSManager().getComponent<SnakePlayer>(eSnake);
    auto& head = player->body.front();

    auto ePickup = getGame()->getEntityWithTag(SNAKE_PICKUP);
    auto pickup = getGame()->getECSManager().getComponent<SnakePickup>(ePickup);

    if (head.x == pickup->position.x &&
        head.y == pickup->position.y)
    {
        addBody();
        getGame()->getECSManager().removeEntity(ePickup);
        createPickup();
    }
}

void SceneSnake::addBody()
{
    auto eSnake = getGame()->getEntityWithTag(SNAKE_PLAYER);
    auto player = getGame()->getECSManager().getComponent<SnakePlayer>(eSnake);

    auto& lastBodyPos = player->body.back();
    player->body.push_back(Vec2(lastBodyPos.x, lastBodyPos.y));
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

void SceneSnake::renderPlayer(sf::RenderWindow& window)
{
    auto eSnake = getGame()->getEntityWithTag(SNAKE_PLAYER);
    auto player = getGame()->getECSManager().getComponent<SnakePlayer>(eSnake);
    auto& body = player->body;

    if (body.empty())
    {
        return;
    }

    for (int i = 1; i < body.size(); i++)
    {
        auto& tail = body.at(i);
        sf::RectangleShape snakeTail(sf::Vector2f(cellSize, cellSize));
        snakeTail.setFillColor(snakeTailColor);
        snakeTail.setPosition(tail.x * cellSize, tail.y * cellSize);
        window.draw(snakeTail);
    }

    auto& head = body.front();
    sf::RectangleShape snakeHead(sf::Vector2f(cellSize, cellSize));
    snakeHead.setFillColor(snakeHeadColor);
    snakeHead.setPosition(head.x * cellSize, head.y * cellSize);
    window.draw(snakeHead);
}

void SceneSnake::renderPickup(sf::RenderWindow& window)
{
    auto ePickup = getGame()->getEntityWithTag(SNAKE_PICKUP);
    auto pickup = getGame()->getECSManager().getComponent<SnakePickup>(ePickup);

    sf::RectangleShape snakeHead(sf::Vector2f(cellSize, cellSize));
    snakeHead.setFillColor(pickipColor);
    snakeHead.setPosition(pickup->position.x * cellSize, pickup->position.y * cellSize);
    window.draw(snakeHead);
}