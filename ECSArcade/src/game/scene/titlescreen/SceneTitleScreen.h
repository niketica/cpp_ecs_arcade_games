#pragma once

#include <vector>
#include <string>
#include "../common/Scene.h"
#include "../common/Animation.h"
#include "../../engine/Game.h"
#include "../asteroids/SceneAsteroids.h"

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

class SceneTitleScreen : public Scene
{
public:
    SceneTitleScreen(Game* game);

    void init();
    void input();
    void update();
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(100, 100, 255);
    const sf::Color activeMenuItemColor = sf::Color(255,255,255);
    const sf::Color inactiveMenuItemColor = sf::Color(180, 180, 180);

    AnimationController animationController;

    void createTitle();
    void createMenuItems();
    int getNextActiveMenuItemIndex();
    int getPreviousActiveMenuItemIndex();
    void updateMenuItems(int activeIndex);
};