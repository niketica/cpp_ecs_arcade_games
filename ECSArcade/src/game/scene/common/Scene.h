#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

class Game;

class Scene
{
public:
    explicit Scene(Game* game);
    virtual ~Scene() = default;

    virtual void init() = 0;
    virtual void input() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render(sf::RenderWindow& window) = 0;

    Game* getGame();

private:
    Game* game = nullptr;
};