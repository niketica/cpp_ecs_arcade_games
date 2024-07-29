#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Game;

class Scene
{
public:
    Scene(Game* game);

    virtual void init() = 0;
    virtual void input() = 0;
    virtual void update() = 0;
    virtual void render(sf::RenderWindow& window) = 0;

protected:
    Game* game = nullptr;
};