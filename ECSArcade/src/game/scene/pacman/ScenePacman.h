#pragma once

#include "../common/Scene.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"

enum PacmanAction
{
    PACMAN_UP,
    PACMAN_DOWN,
    PACMAN_LEFT,
    PACMAN_RIGHT
};

class ScenePacman : public Scene
{
public:
    ScenePacman(Game* game);

    void init();
    void input();
    void update();
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);

    void createActionList();
};