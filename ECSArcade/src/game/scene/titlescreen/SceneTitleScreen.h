#pragma once

#include <vector>
#include <string>

#include "game/engine/Game.h"
#include "game/scene/common/Scene.h"
#include "game/scene/common/Animation.h"
#include "game/scene/asteroids/SceneAsteroids.h"
#include "game/scene/snake/SceneSnake.h"
#include "game/scene/tetris/SceneTetris.h"
#include "game/scene/pacman/ScenePacman.h"

struct MenuItem
{
    int index = 0;
    bool active = false;
    sf::Text text;
};

struct Title
{
    sf::Text text;
};

enum TitleTag
{
    TITLE_BACKGROUND
};

class SceneTitleScreen : public Scene
{
public:
    SceneTitleScreen(Game* game);

    void init();
    void input();
    void update(float deltaTime);
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(100, 100, 255);
    const sf::Color titleColor = sf::Color(151, 0, 252);
    const sf::Color activeMenuItemColor = sf::Color(245, 127, 145);
    const sf::Color inactiveMenuItemColor = sf::Color(197, 198, 240);

    AnimationController animationController;

    void createTitle();
    void createMenuItems();
    void createBackground();

    int getNextActiveMenuItemIndex();
    int getPreviousActiveMenuItemIndex();
    void updateMenuItems(int activeIndex);
};