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
    SNAKE
};

struct SnakePlayer
{
    Vec2 head{ 2, 2 };
    std::vector<Vec2> body;
    SnakeAction currentAction = SNAKE_RIGHT;
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
    const int columns = 9;
    const int rows = 7;
    const int cellSize = 64;
    const int movementCooldown = 30;
    int currentMovementCooldown = 0;

    void createActionList();
    void renderGrid(sf::RenderWindow& window);

    void moveSnakeLeft(SnakePlayer& player);
    void moveSnakeRight(SnakePlayer& player);
    void moveSnakeUp(SnakePlayer& player);
    void moveSnakeDown(SnakePlayer& player);
};