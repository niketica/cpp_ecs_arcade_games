#pragma once

#include "memory"
#include <cmath>
#include "../common/Scene.h"
#include "../common/Animation.h"
#include "../common/RandomNumberGenerator.h"
#include "../../engine/Game.h"
#include "../titlescreen/SceneTitleScreen.h"
#include "../../common/Vec2.h"

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

enum class AsteroidTag
{
    UNDEFINED_ENTITY,
    PLAYER,
    ASTEROIDS_BACKGROUND,
    ASTEROID,
    EXPLOSION,
    LASER
};

enum class AsteroidsAction
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
    explicit SceneAsteroids(Game* game);

    void init() override;
    void input() override;
    void update(float deltaTime) override;
    void render(sf::RenderWindow& window) override;

private:
    const double M_PI = 3.14159265358979323846;
    const sf::Color windowClearColor = sf::Color(0, 0, 0);
    const float playerSpeedIncrement = 1.f;
    const float maxPlayerSpeed = 5.f;
    const RandomNumberGenerator rand = {};

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
    void processAction(AsteroidsAction action, Transform& transform) const;
    void updatePosition(Transform& transform, float deltaTime) const;
    void updatePlayer();
    void updateAsteroid(Entity asteroid);
    void updateLaser(Entity laser);
    void detectPlayerCollision();
    void detectLaserCollision(Entity laser);

    void processKeyPressed(const int key);
    void processKeyReleased(const int key);
    bool isActionPresent(std::vector<AsteroidsAction>& actionList, AsteroidsAction action) const;
    void removeAction(std::vector<AsteroidsAction>& actionList, AsteroidsAction action) const;

    Entity getEntityWithTag(AsteroidTag tag);
    std::vector<Entity> getEntitiesWithTag(AsteroidTag tag);

    void renderBackground(sf::RenderWindow& window, Entity entity);
    void renderAsteroid(sf::RenderWindow& window, Entity entity);
    void renderLaser(sf::RenderWindow& window, Entity entity);
    void renderExplosion(sf::RenderWindow& window, Entity entity);
    void renderPlayer(sf::RenderWindow& window, Entity entity);
};