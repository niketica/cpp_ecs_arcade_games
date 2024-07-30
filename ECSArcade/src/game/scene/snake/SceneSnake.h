#pragma once

#include "../common/Scene.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"

enum SnakeAction
{
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT
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

    void createActionList();
};