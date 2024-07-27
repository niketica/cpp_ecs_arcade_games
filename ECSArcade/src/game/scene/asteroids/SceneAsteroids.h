#pragma once

#include "memory"
#include <cmath>
#include "../common/Scene.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"
#include "../../common/Vec2.h"

#define M_PI 3.14159265358979323846

struct Player
{
    Vec2 position;
    Vec2 velocity;
    float speed = 0.f;
    float degrees = 270.f;
};

enum Tag
{
    UNDEFINED_,
    PLAYER,
    BACKGROUND
};

struct EntityTag
{
    Tag tag = UNDEFINED_;
};

enum Action
{
    THROTTLE,
    ROTATE_LEFT,
    ROTATE_RIGHT
};

class SceneAsteroids : public Scene
{
public:
    SceneAsteroids(Game* game);

    void init();
    void input();
    void update();
    void render(sf::RenderWindow& window);

private:
    const sf::Color windowClearColor = sf::Color(0, 0, 0);
    const float playerSpeedIncrement = 1.f;
    const float maxPlayerSpeed = 5.f;

    void createPlayer();
    void createBackground();
    void createActionList();
    void processActionList(Player& cPlayer);
    void updatePlayerPosition(Player& player, float deltaTime);
};