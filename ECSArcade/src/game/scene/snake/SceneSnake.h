#pragma once

#include "../common/Scene.h"
#include "../../common/Vec2.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"

enum SnakeAction
{
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT
};

enum SnakeTag
{
    SNAKE_PLAYER,
    SNAKE_PICKUP
};

struct SnakePlayer
{
    std::vector<Vec2> body;
    SnakeAction currentAction = SNAKE_RIGHT;
    SnakeAction nextAction = SNAKE_RIGHT;
    bool dead = false;
};

struct SnakePickup
{
    SnakePickup(Vec2 position) : position(position) {}

    Vec2 position{ 0, 0 };
};

class SceneSnake : public Scene
{
public:
    SceneSnake(Game* game);

    void init();
    void input();
    void update();
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);
    const sf::Color snakeHeadColor = sf::Color(0, 200, 0);
    const sf::Color snakeTailColor = sf::Color(0, 100, 0);
    const sf::Color pickipColor = sf::Color(200, 0, 0);

    const int columns = 9;
    const int rows = 7;
    const int cellSize = 64;
    const int movementCooldown = 30;
    int currentMovementCooldown = 0;

    void createActionList();
    void renderGrid(sf::RenderWindow& window);
    void renderPlayer(sf::RenderWindow& window);
    void renderPickup(sf::RenderWindow& window);

    void moveSnakeLeft(SnakePlayer& player);
    void moveSnakeRight(SnakePlayer& player);
    void moveSnakeUp(SnakePlayer& player);
    void moveSnakeDown(SnakePlayer& player);
    void updateBody(SnakePlayer& player);
    void checkSnakeCollision(SnakePlayer& player);

    void createPlayer();
    void createPickup();
    void checkPickup();
    void addBody();
    
    Vec2 getRandomPosition();
};