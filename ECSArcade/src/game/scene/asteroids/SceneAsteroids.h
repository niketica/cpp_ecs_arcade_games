#pragma once

#include "memory"
#include <cmath>
#include "../common/Scene.h"
#include "../common/Animation.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"
#include "../../common/Vec2.h"

#define M_PI 3.14159265358979323846

struct Transform
{
    Vec2 position;
    Vec2 velocity;
    float speed = 0.f;
    float degrees = 270.f;
    float scale = 1.f;
};

struct BoundingBox
{
    float radius = 0.f;
};

enum AsteroidTag
{
    UNDEFINED_ENTITY,
    PLAYER,
    ASTEROIDS_BACKGROUND,
    ASTEROID,
    EXPLOSION,
    LASER
};

enum AsteroidsAction
{
    THROTTLE,
    ROTATE_LEFT,
    ROTATE_RIGHT,
    SHOOT
};

struct PlayerStats
{
    bool alive = true;
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

    int asteroidCount = 0;
    int asteroidMax = 20;

    int laserCooldown = 10;
    int currentlaserCooldown = 0;

    bool drawTextures = true;
    bool drawCollision = false;

    AnimationController animationController;

    void createPlayer();
    void createBackground();
    void createAsteroid();
    void createActionList();
    void createExplosion(Vec2 position, float scale);
    void createLaser();

    void processActionList(Transform& transform);
    void updatePosition(Transform& transform, float deltaTime);
    void updatePlayer();
    void updateAsteroid(Entity asteroid);
    void updateLaser(Entity laser);
    void detectPlayerCollision();
    void detectLaserCollision(Entity laser);
};