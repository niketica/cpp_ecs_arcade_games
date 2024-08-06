#include "SceneAsteroids.h"

SceneAsteroids::SceneAsteroids(Game* game) : Scene(game) {}

void SceneAsteroids::init()
{
    getGame()->getECSManager().reset();

    createActionList();
    createPlayer();
    createBackground();

    asteroidCount = 0;
    asteroidMax = 20;
    laserCooldown = 10;
    currentlaserCooldown = 0;
}

void SceneAsteroids::createPlayer()
{
    sf::Sprite sprite;
    sprite = getGame()->getAssetManager().getSprite("space_ship");

    auto playerSize = sprite.getTexture()->getSize();
    auto xPos = getGame()->getWidth() / 2.f - playerSize.x / 2.f;
    auto yPos = getGame()->getHeight() / 2.f - playerSize.y / 2.f;

    Transform transform;
    transform.position = { xPos, yPos };

    BoundingBox boundingBox{ 24 };

    Entity entity = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<AsteroidTag>(entity, AsteroidTag::PLAYER);
    getGame()->getECSManager().addComponent<Transform>(entity, transform);
    getGame()->getECSManager().addComponent<sf::Sprite>(entity, sprite);
    getGame()->getECSManager().addComponent<BoundingBox>(entity, boundingBox);
    getGame()->getECSManager().addComponent<PlayerStats>(entity, PlayerStats());
}

void SceneAsteroids::createAsteroid()
{
    asteroidCount++;

    sf::Sprite sprite;
    sprite = getGame()->getAssetManager().getSprite("asteroid");

    int textureWidth = sprite.getTexture()->getSize().x;
    int textureHeight = sprite.getTexture()->getSize().y;

    Transform transform;
    if (rand.getRandomNumber(1) == 1)
    {
        float xPos = rand.getRandomNumber(getGame()->getWidth());
        if (rand.getRandomNumber(1) == 1)
        {
            transform.position = { xPos, textureHeight * -1.f };
        }
        else
        {
            transform.position = { xPos, (float) (getGame()->getHeight() + textureHeight) };
        }
    }
    else
    {
        float yPos = rand.getRandomNumber(getGame()->getHeight());
        if (rand.getRandomNumber(1) == 1)
        {
            transform.position = { textureWidth * -1.f, yPos };
        }
        else
        {
            transform.position = { (float) (getGame()->getWidth() + textureWidth), yPos};
        }
    }

    transform.degrees = rand.getRandomNumber(270);
    transform.speed = rand.getRandomNumber(4) + 2.f;
    transform.scale = (rand.getRandomNumber(80) + 20) * .01f;

    BoundingBox boundingBox;
    boundingBox.radius = (textureWidth / 2.f) - 12.f; // Minus some margin since not the entire texture is an asteroid.

    Entity entity = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<AsteroidTag>(entity, AsteroidTag::ASTEROID);
    getGame()->getECSManager().addComponent<sf::Sprite>(entity, sprite);
    getGame()->getECSManager().addComponent<Transform>(entity, transform);
    getGame()->getECSManager().addComponent<BoundingBox>(entity, boundingBox);
}

void SceneAsteroids::createBackground()
{
    sf::Sprite backgroundSprite;
    backgroundSprite = getGame()->getAssetManager().getSprite("space_background");
    backgroundSprite.setScale(0.5f, 0.5f);

    Entity eBackground = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<AsteroidTag>(eBackground, AsteroidTag::ASTEROIDS_BACKGROUND);
    getGame()->getECSManager().addComponent<sf::Sprite>(eBackground, backgroundSprite);
}

void SceneAsteroids::createExplosion(Vec2 position, float scale)
{
    Transform transform;
    transform.position = { position.x, position.y };
    transform.degrees = 90.f;
    transform.scale = scale;

    sf::Sprite sprite;
    sprite = getGame()->getAssetManager().getSprite("explosion_realistic");

    Entity entity = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<AsteroidTag>(entity, AsteroidTag::EXPLOSION);
    getGame()->getECSManager().addComponent<Transform>(entity, transform);
    getGame()->getECSManager().addComponent<sf::Sprite>(entity, sprite);

    auto cSprite = getGame()->getECSManager().getComponent<sf::Sprite>(entity);
    Animation explosionAnimation = {
    3,   // Number of rows in the spritesheet
    6,   // Number of columns in the spritesheet
    72,  // Width of each frame (example value)
    101, // Height of each frame (example value)
    0,   // Current frame index
    17,  // Total number of frames available
    cSprite.get() // Pointer to the sprite object
    };
    animationController.updateAnimationFrame(explosionAnimation);
    getGame()->getECSManager().addComponent<Animation>(entity, explosionAnimation);
}

Entity SceneAsteroids::getEntityWithTag(AsteroidTag tag)
{
    auto entities = getGame()->getECSManager().getEntitiesWithComponent<AsteroidTag>();

    Entity returnEntity = NULL;
    for (auto entity : entities)
    {
        auto entityTag = getGame()->getECSManager().getComponent<AsteroidTag>(entity);

        if (*entityTag == tag)
        {
            returnEntity = entity;
        }
    }

    if (returnEntity == NULL)
    {
        std::cerr << "ERROR!!! Entity not found with tag." << std::endl;
        exit(-1);
    }

    return returnEntity;
}

std::vector<Entity> SceneAsteroids::getEntitiesWithTag(AsteroidTag tag)
{
    auto entities = getGame()->getECSManager().getEntitiesWithComponent<AsteroidTag>();
    std::vector<Entity> filtered;

    for (auto entity : entities)
    {
        auto entityTag = getGame()->getECSManager().getComponent<AsteroidTag>(entity);
        if (*entityTag == tag)
        {
            filtered.push_back(entity);
        }
    }

    return filtered;
}

void SceneAsteroids::createLaser()
{
    auto player = getEntityWithTag(AsteroidTag::PLAYER);
    auto playerStats = getGame()->getECSManager().getComponent<PlayerStats>(player);
    if (!playerStats->alive || currentlaserCooldown > 0)
    {
        return;
    }
    currentlaserCooldown = laserCooldown;

    auto playerTransform = getGame()->getECSManager().getComponent<Transform>(player);

    Entity entity = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<AsteroidTag>(entity, AsteroidTag::LASER);

    Transform transform;
    transform.position = { playerTransform->position.x, playerTransform->position.y };
    transform.velocity = { 0, 0 };
    transform.degrees = playerTransform->degrees;
    transform.speed = 4.f;
    getGame()->getECSManager().addComponent<Transform>(entity, transform);

    sf::Sprite sprite;
    sprite = getGame()->getAssetManager().getSprite("laser");
    getGame()->getECSManager().addComponent<sf::Sprite>(entity, sprite);

    BoundingBox boundingBox;
    boundingBox.radius = 24; // Should be a rectangle, but... whatever.
    getGame()->getECSManager().addComponent<BoundingBox>(entity, boundingBox);
}

void SceneAsteroids::createActionList()
{
    Entity eAction = getGame()->getECSManager().addEntity();
    std::vector<AsteroidsAction> actionList;
    getGame()->getECSManager().addComponent<std::vector<AsteroidsAction>>(eAction, actionList);
}

void SceneAsteroids::input()
{
    auto keyInputs = getGame()->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<AsteroidsAction>>();
            if (keyInput->keyType == W)
            {
                if (! (std::find(actionList->begin(), actionList->end(), AsteroidsAction::THROTTLE) != actionList->end()))
                {
                    actionList->push_back(AsteroidsAction::THROTTLE);
                }
            }
            else if (keyInput->keyType == A)
            {
                if (!(std::find(actionList->begin(), actionList->end(), AsteroidsAction::ROTATE_LEFT) != actionList->end()))
                {
                    actionList->push_back(AsteroidsAction::ROTATE_LEFT);
                }
            }
            else if (keyInput->keyType == D)
            {
                if (!(std::find(actionList->begin(), actionList->end(), AsteroidsAction::ROTATE_RIGHT) != actionList->end()))
                {
                    actionList->push_back(AsteroidsAction::ROTATE_RIGHT);
                }
            }
            else if (keyInput->keyType == C)
            {
                drawCollision = !drawCollision;
            }
            else if (keyInput->keyType == T)
            {
                drawTextures = !drawTextures;
            }
            else if (keyInput->keyType == R)
            {
                init();
            }
            else if (keyInput->keyType == SPACE_KEY)
            {
                createLaser();
            }
            else if (keyInput->keyType == ESCAPE_KEY)
            {
                std::shared_ptr<Scene> scene = std::make_shared<SceneTitleScreen>(getGame());
                getGame()->loadScene(scene);
                return;
            }
        }
        else if (keyInput->inputType == RELEASED)
        {
            auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<AsteroidsAction>>();
            if (keyInput->keyType == W)
            {
                actionList->erase(std::remove(actionList->begin(), actionList->end(), AsteroidsAction::THROTTLE), actionList->end());
            }
            if (keyInput->keyType == A)
            {
                actionList->erase(std::remove(actionList->begin(), actionList->end(), AsteroidsAction::ROTATE_LEFT), actionList->end());
            }
            if (keyInput->keyType == D)
            {
                actionList->erase(std::remove(actionList->begin(), actionList->end(), AsteroidsAction::ROTATE_RIGHT), actionList->end());
            }
        }
    }
}

void SceneAsteroids::update(float deltaTime)
{
    auto explosions = getEntitiesWithTag(AsteroidTag::EXPLOSION);
    for (auto explosion : explosions)
    {
        auto explosionAnimation = getGame()->getECSManager().getComponent<Animation>(explosion);
        animationController.updateAnimation(*explosionAnimation, deltaTime);
    }

    if (asteroidCount < asteroidMax)
    {
        createAsteroid();
    }

    auto asteroids = getEntitiesWithTag(AsteroidTag::ASTEROID);
    for (auto asteroid : asteroids)
    {
        updateAsteroid(asteroid);
    }

    auto player = getEntityWithTag(AsteroidTag::PLAYER);
    auto playerStats = getGame()->getECSManager().getComponent<PlayerStats>(player);
    if (playerStats->alive)
    {
        updatePlayer();
        detectPlayerCollision();
    }

    if (currentlaserCooldown > 0)
    {
        currentlaserCooldown--;
    }
    auto lasers = getEntitiesWithTag(AsteroidTag::LASER);
    for (auto laser : lasers)
    {
        updateLaser(laser);
    }
    lasers = getEntitiesWithTag(AsteroidTag::LASER);
    for (auto laser : lasers)
    {
        detectLaserCollision(laser);
    }
}

void SceneAsteroids::updatePlayer()
{
    Entity player = getEntityWithTag(AsteroidTag::PLAYER);
    auto transform = getGame()->getECSManager().getComponent<Transform>(player);
    auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(player);

    processActionList(*transform);
    updatePosition(*transform, 1.f);

    int textureWidth = sprite->getTexture()->getSize().x;
    int textureHeight = sprite->getTexture()->getSize().y;

    auto& position = transform->position;
    if (position.x < -textureWidth)
    {
        position.x = getGame()->getWidth();
    }
    else if (position.x > getGame()->getWidth())
    {
        position.x = 1 - textureWidth;
    }
    else if (position.y < -textureHeight)
    {
        position.y = getGame()->getHeight();
    }
    else if (position.y > getGame()->getHeight())
    {
        position.y = 1 - textureHeight;
    }

    if (transform->speed > 0)
    {
        transform->speed -= 0.05f;

        if (transform->speed < 0)
        {
            transform->speed = 0;
        }
    }
}

void SceneAsteroids::updateAsteroid(Entity asteroid)
{
    auto transform = getGame()->getECSManager().getComponent<Transform>(asteroid);
    auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(asteroid);

    updatePosition(*transform, 1.f);

    int textureWidth = sprite->getTexture()->getSize().x;
    int textureHeight = sprite->getTexture()->getSize().y;
    int borderMargin = textureWidth * 2;
    auto& position = transform->position;
    if (position.x < -borderMargin || position.x > getGame()->getWidth() + borderMargin ||
        position.y < -borderMargin || position.y > getGame()->getHeight() + borderMargin)
    {
        asteroidCount--;
        getGame()->getECSManager().removeEntity(asteroid);

        createAsteroid();
    }

    sprite->rotate(1.f);
}

void SceneAsteroids::updateLaser(Entity laser)
{
    auto transform = getGame()->getECSManager().getComponent<Transform>(laser);
    auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(laser);

    updatePosition(*transform, transform->speed);

    int textureWidth = sprite->getTexture()->getSize().x;
    int textureHeight = sprite->getTexture()->getSize().y;
    int borderMargin = textureWidth * 2;
    auto& position = transform->position;
    if (position.x < -borderMargin || position.x > getGame()->getWidth() + borderMargin ||
        position.y < -borderMargin || position.y > getGame()->getHeight() + borderMargin)
    {
        getGame()->getECSManager().removeEntity(laser);
    }
}

void SceneAsteroids::processActionList(Transform& transform)
{
    auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<AsteroidsAction>>();

    float rotationSpeed = 6.f;

    for (auto& action : *actionList)
    {
        switch (action)
        {
        case AsteroidsAction::THROTTLE:
            {
                if (transform.speed < maxPlayerSpeed)
                {
                    transform.speed += playerSpeedIncrement;
                    if (transform.speed > maxPlayerSpeed) transform.speed = maxPlayerSpeed;
                }
                break;
            }
            case AsteroidsAction::ROTATE_LEFT:
            {
                transform.degrees -= rotationSpeed;
                if (transform.degrees < 0)
                {
                    transform.degrees = 360 - transform.degrees;
                }
                break;
            }
            case AsteroidsAction::ROTATE_RIGHT:
            {
                transform.degrees += rotationSpeed;
                if (transform.degrees > 360)
                {
                    transform.degrees = transform.degrees - 360;
                }
                break;
            }
        }
    }
}

void SceneAsteroids::updatePosition(Transform& transform, float deltaTime) {
    float radians = transform.degrees * (M_PI / 180.0f);

    transform.velocity.x = std::cos(radians) * transform.speed;
    transform.velocity.y = std::sin(radians) * transform.speed;
    transform.position = transform.position + (transform.velocity * deltaTime);
}

void SceneAsteroids::detectPlayerCollision()
{
    auto player = getEntityWithTag(AsteroidTag::PLAYER);
    auto playerTransform = getGame()->getECSManager().getComponent<Transform>(player);
    auto playerBoundingBox = getGame()->getECSManager().getComponent<BoundingBox>(player);

    auto asteroids = getEntitiesWithTag(AsteroidTag::ASTEROID);
    for (auto asteroid : asteroids)
    {
        auto asteroidTransform = getGame()->getECSManager().getComponent<Transform>(asteroid);
        auto asteroidBoundingBox = getGame()->getECSManager().getComponent<BoundingBox>(asteroid);

        Vec2 deltaV = playerTransform->position - asteroidTransform->position;
        float length = deltaV.length();

        if (length <= playerBoundingBox->radius || length <= asteroidBoundingBox->radius)
        {
            auto playerStats = getGame()->getECSManager().getComponent<PlayerStats>(player);
            playerStats->alive = false;
            createExplosion(playerTransform->position, playerTransform->scale);
        }
    }
}

void SceneAsteroids::detectLaserCollision(Entity laser)
{
    auto laserTransform = getGame()->getECSManager().getComponent<Transform>(laser);
    auto laserBoundingBox = getGame()->getECSManager().getComponent<BoundingBox>(laser);

    auto asteroids = getEntitiesWithTag(AsteroidTag::ASTEROID);
    for (auto asteroid : asteroids)
    {
        auto asteroidTransform = getGame()->getECSManager().getComponent<Transform>(asteroid);
        auto asteroidBoundingBox = getGame()->getECSManager().getComponent<BoundingBox>(asteroid);

        Vec2 deltaV = laserTransform->position - asteroidTransform->position;
        float length = deltaV.length();

        if (length <= laserBoundingBox->radius || length <= asteroidBoundingBox->radius)
        {
            getGame()->getECSManager().removeEntity(asteroid);
            asteroidCount--;
            createExplosion(asteroidTransform->position, asteroidTransform->scale);

            getGame()->getECSManager().removeEntity(laser);
            return;
        }
    }
}

void SceneAsteroids::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    auto entities = getGame()->getECSManager().getEntitiesWithComponent<AsteroidTag>();

    for (auto& entity : entities)
    {
        auto tag = getGame()->getECSManager().getComponent<AsteroidTag>(entity);
        if (*tag == AsteroidTag::ASTEROIDS_BACKGROUND)
        {
            if (drawTextures)
            {
                auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(entity);
                window.draw(*sprite);
            }
        }
    }

    for (auto& entity : entities)
    {
        auto tag = getGame()->getECSManager().getComponent<AsteroidTag>(entity);
        if (*tag == AsteroidTag::ASTEROID)
        {
            auto transform = getGame()->getECSManager().getComponent<Transform>(entity);

            if (drawTextures)
            {
                auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(entity);

                sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
                sprite->setOrigin(size.x / 2.f, size.y / 2.f);
                sprite->setPosition(transform->position.x, transform->position.y);
                sprite->setScale(transform->scale, transform->scale);

                window.draw(*sprite);
            }

            if (drawCollision)
            {
                auto boundingBox = getGame()->getECSManager().getComponent<BoundingBox>(entity);

                sf::CircleShape boundingCircle(boundingBox->radius);
                boundingCircle.setFillColor(sf::Color(0, 0, 0, 0));
                boundingCircle.setOutlineThickness(1);
                boundingCircle.setOutlineColor(sf::Color(250, 0, 0));
                boundingCircle.setOrigin(boundingBox->radius, boundingBox->radius);
                boundingCircle.setPosition(transform->position.x, transform->position.y);
                boundingCircle.setScale(transform->scale, transform->scale);

                window.draw(boundingCircle);
            }
        }
    }

    auto lasers = getEntitiesWithTag(AsteroidTag::LASER);
    for (auto laser : lasers)
    {
        auto transform = getGame()->getECSManager().getComponent<Transform>(laser);
        if (drawTextures)
        {
            auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(laser);

            sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
            sprite->setOrigin(size.x / 2.0f, size.y / 2.0f);
            sprite->setPosition(transform->position.x, transform->position.y);
            sprite->setRotation(transform->degrees - 90.f);
            sprite->setScale(transform->scale, transform->scale);

            window.draw(*sprite);
        }

        if (drawCollision)
        {
            auto boundingBox = getGame()->getECSManager().getComponent<BoundingBox>(laser);

            sf::CircleShape boundingCircle(boundingBox->radius);
            boundingCircle.setFillColor(sf::Color(0, 0, 0, 0));
            boundingCircle.setOutlineThickness(1);
            boundingCircle.setOutlineColor(sf::Color(0, 250, 250));
            boundingCircle.setOrigin(boundingBox->radius, boundingBox->radius);
            boundingCircle.setPosition(transform->position.x, transform->position.y);
            boundingCircle.setScale(transform->scale, transform->scale);

            window.draw(boundingCircle);
        }
    }

    auto explosions = getEntitiesWithTag(AsteroidTag::EXPLOSION);
    for (auto explosion : explosions)
    {
        if (drawTextures)
        {
            auto explTransform = getGame()->getECSManager().getComponent<Transform>(explosion);
            auto explosionAnimation = getGame()->getECSManager().getComponent<Animation>(explosion);

            explosionAnimation->sprite->setOrigin(explosionAnimation->width / 2.0f, explosionAnimation->height / 2.0f);
            explosionAnimation->sprite->setPosition(explTransform->position.x, explTransform->position.y);
            explosionAnimation->sprite->setRotation(explTransform->degrees);
            explosionAnimation->sprite->setScale(explTransform->scale, explTransform->scale);

            if (explosionAnimation->finished)
            {
                getGame()->getECSManager().removeEntity(explosion);
            }
            else
            {
                window.draw(*explosionAnimation->sprite);
            }
        }
    }

    auto player = getEntityWithTag(AsteroidTag::PLAYER);
    auto playerStats = getGame()->getECSManager().getComponent<PlayerStats>(player);
    if (playerStats->alive)
    {
        auto playerTransform = getGame()->getECSManager().getComponent<Transform>(player);

        if (drawTextures)
        {
            auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(player);

            sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
            sprite->setOrigin(size.x / 2.0f, size.y / 2.0f);
            sprite->setPosition(playerTransform->position.x, playerTransform->position.y);
            sprite->setRotation(playerTransform->degrees - 90.f);
            sprite->setScale(playerTransform->scale, playerTransform->scale);

            window.draw(*sprite);
        }

        if (drawCollision)
        {
            auto boundingBox = getGame()->getECSManager().getComponent<BoundingBox>(player);

            sf::CircleShape boundingCircle(boundingBox->radius);
            boundingCircle.setFillColor(sf::Color(0, 0, 0, 0));
            boundingCircle.setOutlineThickness(1);
            boundingCircle.setOutlineColor(sf::Color(100, 250, 50));
            boundingCircle.setOrigin(boundingBox->radius, boundingBox->radius);
            boundingCircle.setPosition(playerTransform->position.x, playerTransform->position.y);
            boundingCircle.setScale(playerTransform->scale, playerTransform->scale);

            window.draw(boundingCircle);
        }
    }

    window.display();
}