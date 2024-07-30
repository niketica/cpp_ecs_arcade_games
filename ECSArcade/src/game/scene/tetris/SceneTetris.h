#pragma once

#include "../common/Scene.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"

enum TetrisAction
{
    TETRIS_UP,
    TETRIS_DOWN,
    TETRIS_LEFT,
    TETRIS_RIGHT
};

class SceneTetris : public Scene
{
public:
    SceneTetris(Game* game);

    void init();
    void input();
    void update();
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);

    void createActionList();
};