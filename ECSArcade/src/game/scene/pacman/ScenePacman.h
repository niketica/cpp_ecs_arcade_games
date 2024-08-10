#pragma once

#include "game/engine/Game.h"
#include "game/scene/common/Scene.h"
#include "game/scene/titlescreen/SceneTitleScreen.h"

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
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);

    void createActionList();
};