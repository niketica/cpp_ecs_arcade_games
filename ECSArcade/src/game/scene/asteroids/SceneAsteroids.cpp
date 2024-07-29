#include "SceneAsteroids.h"

SceneAsteroids::SceneAsteroids(Game* game) : Scene(game) 
{
    init();
}

void SceneAsteroids::init()
{
    std::cout << "SceneAsteroids - init" << std::endl;

    game->getECSManager().reset();

    srand(time(NULL));

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
    Entity entity = game->getECSManager().addEntity();
    EntityTag entityTag;
    entityTag.value = PLAYER;
    game->getECSManager().addComponent<EntityTag>(entity, entityTag);

    Transform transform;
    transform.position = { 100, 200 };
    transform.velocity = { 0, 0 };
    game->getECSManager().addComponent<Transform>(entity, transform);

    sf::Sprite sprite;
    sprite = game->getAssetManager().getSprite("space_ship");
    game->getECSManager().addComponent<sf::Sprite>(entity, sprite);

    BoundingBox boundingBox;
    boundingBox.radius = 24;
    game->getECSManager().addComponent<BoundingBox>(entity, boundingBox);

    PlayerStats stats;
    game->getECSManager().addComponent<PlayerStats>(entity, stats);
}

void SceneAsteroids::createAsteroid()
{
    Entity entity = game->getECSManager().addEntity();
    EntityTag entityTag;
    entityTag.value = ASTEROID;
    game->getECSManager().addComponent<EntityTag>(entity, entityTag);

    sf::Sprite sprite;
    sprite = game->getAssetManager().getSprite("asteroid");

    int textureWidth = sprite.getTexture()->getSize().x;
    int textureHeight = sprite.getTexture()->getSize().y;

    Transform transform;
    if (rand() % 2)
    {
        float xPos = rand() % game->getWidth();
        if (rand() % 2)
        {
            transform.position = { xPos, textureHeight * -1.f };
        }
        else
        {
            transform.position = { xPos, (float) (game->getHeight() + textureHeight) };
        }
    }
    else
    {
        float yPos = rand() % game->getHeight();
        if (rand() % 2)
        {
            transform.position = { textureWidth * -1.f, yPos };
        }
        else
        {
            transform.position = { (float) (game->getWidth() + textureWidth), yPos};
        }
    }

    transform.degrees = rand() % 270;
    transform.velocity = { 0, 0 };
    transform.speed = (rand() % 4) + 2.f;
    transform.scale = ((rand() % 80) + 20) * .01f;

    game->getECSManager().addComponent<sf::Sprite>(entity, sprite);
    game->getECSManager().addComponent<Transform>(entity, transform);

    BoundingBox boundingBox;
    boundingBox.radius = (textureWidth / 2.f) - 12.f; // Minus some margin since not the entire texture is an asteroid.
    game->getECSManager().addComponent<BoundingBox>(entity, boundingBox);

    asteroidCount++;
}

void SceneAsteroids::createBackground()
{
    Entity eBackground = game->getECSManager().addEntity();
    EntityTag backgroundTag;
    backgroundTag.value = BACKGROUND;
    game->getECSManager().addComponent<EntityTag>(eBackground, backgroundTag);

    sf::Sprite backgroundSprite;
    backgroundSprite = game->getAssetManager().getSprite("space_background");
    backgroundSprite.setScale(0.5f, 0.5f);
    game->getECSManager().addComponent<sf::Sprite>(eBackground, backgroundSprite);
}

void SceneAsteroids::createExplosion(Vec2 position)
{
    Entity entity = game->getECSManager().addEntity();
    EntityTag entityTag;
    entityTag.value = EXPLOSION;
    game->getECSManager().addComponent<EntityTag>(entity, entityTag);

    Transform transform;
    transform.position = { position.x, position.y };
    transform.velocity = { 0, 0 };
    transform.degrees = 90.f;
    game->getECSManager().addComponent<Transform>(entity, transform);

    sf::Sprite sprite;
    sprite = game->getAssetManager().getSprite("explosion_realistic");
    game->getECSManager().addComponent<sf::Sprite>(entity, sprite);

    Animation explosionAnimation = {
    3,   // Number of rows in the spritesheet
    6,   // Number of columns in the spritesheet
    72,  // Width of each frame (example value)
    101, // Height of each frame (example value)
    0,   // Current frame index
    17,  // Total number of frames available
    &sprite  // Pointer to the sprite object
    };
    game->getECSManager().addComponent<Animation>(entity, explosionAnimation);
}

void SceneAsteroids::createLaser()
{
    auto player = getEntityWithTag(PLAYER);
    auto playerStats = game->getECSManager().getComponent<PlayerStats>(player);
    if (!playerStats->alive || currentlaserCooldown > 0)
    {
        return;
    }
    currentlaserCooldown = laserCooldown;

    auto playerTransform = game->getECSManager().getComponent<Transform>(player);

    Entity entity = game->getECSManager().addEntity();
    EntityTag entityTag;
    entityTag.value = LASER;
    game->getECSManager().addComponent<EntityTag>(entity, entityTag);

    Transform transform;
    transform.position = { playerTransform->position.x, playerTransform->position.y };
    transform.velocity = { 0, 0 };
    transform.degrees = playerTransform->degrees;
    transform.speed = 4.f;
    game->getECSManager().addComponent<Transform>(entity, transform);

    sf::Sprite sprite;
    sprite = game->getAssetManager().getSprite("laser");
    game->getECSManager().addComponent<sf::Sprite>(entity, sprite);

    BoundingBox boundingBox;
    boundingBox.radius = 24;
    game->getECSManager().addComponent<BoundingBox>(entity, boundingBox);
}

void SceneAsteroids::createActionList()
{
    Entity eAction = game->getECSManager().addEntity();
    std::vector<Action> actionList;
    game->getECSManager().addComponent<std::vector<Action>>(eAction, actionList);
}

void SceneAsteroids::input()
{
    auto keyInputs = game->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = game->getECSManager().getAnyComponent<std::vector<Action>>();
            if (keyInput->keyType == W)
            {
                if (! (std::find(actionList->begin(), actionList->end(), THROTTLE) != actionList->end()))
                {
                    actionList->push_back(THROTTLE);
                }
            }
            else if (keyInput->keyType == S)
            {

            }
            else if (keyInput->keyType == A)
            {
                if (!(std::find(actionList->begin(), actionList->end(), ROTATE_LEFT) != actionList->end()))
                {
                    actionList->push_back(ROTATE_LEFT);
                }
            }
            else if (keyInput->keyType == D)
            {
                if (!(std::find(actionList->begin(), actionList->end(), ROTATE_RIGHT) != actionList->end()))
                {
                    actionList->push_back(ROTATE_RIGHT);
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
                std::shared_ptr<Scene> scene = std::make_shared<SceneTitleScreen>(game);
                game->loadScene(scene);
                return;
            }
        }
        else if (keyInput->inputType == RELEASED)
        {
            auto actionList = game->getECSManager().getAnyComponent<std::vector<Action>>();
            if (keyInput->keyType == W)
            {
                actionList->erase(std::remove(actionList->begin(), actionList->end(), THROTTLE), actionList->end());
            }
            if (keyInput->keyType == A)
            {
                actionList->erase(std::remove(actionList->begin(), actionList->end(), ROTATE_LEFT), actionList->end());
            }
            if (keyInput->keyType == D)
            {
                actionList->erase(std::remove(actionList->begin(), actionList->end(), ROTATE_RIGHT), actionList->end());
            }
        }
    }
}

void SceneAsteroids::update()
{
    if (asteroidCount < asteroidMax)
    {
        createAsteroid();
    }

    auto asteroids = getEntitiesWithTag(ASTEROID);
    for (auto asteroid : asteroids)
    {
        updateAsteroid(asteroid);
    }

    auto player = getEntityWithTag(PLAYER);
    auto playerStats = game->getECSManager().getComponent<PlayerStats>(player);
    if (playerStats->alive)
    {
        updatePlayer();
        detectPlayerCollision();
    }

    if (currentlaserCooldown > 0)
    {
        currentlaserCooldown--;
    }
    auto lasers = getEntitiesWithTag(LASER);
    for (auto laser : lasers)
    {
        updateLaser(laser);
    }
    lasers = getEntitiesWithTag(LASER);
    for (auto laser : lasers)
    {
        detectLaserCollision(laser);
    }
}

Entity SceneAsteroids::getEntityWithTag(AsteroidTag tag)
{
    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();

    Entity returnEntity = NULL;
    for (auto entity : entities)
    {
        auto entityTag = game->getECSManager().getComponent<EntityTag>(entity);

        if (entityTag->value == tag)
        {
            returnEntity = entity;
        }
    }

    if (returnEntity == NULL)
    {
        std::cerr << "ERROR!!! Entity not found with tag " << tag << std::endl;
        game->getECSManager().printComponents();
        exit(-1);
    }

    return returnEntity;
}

std::vector<Entity> SceneAsteroids::getEntitiesWithTag(AsteroidTag tag)
{
    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();
    std::vector<Entity> filtered;

    for (auto entity : entities)
    {
        auto entityTag = game->getECSManager().getComponent<EntityTag>(entity);
        if (entityTag->value == tag)
        {
            filtered.push_back(entity);
        }
    }

    return filtered;
}

void SceneAsteroids::updatePlayer()
{
    Entity player = getEntityWithTag(PLAYER);

    auto transform = game->getECSManager().getComponent<Transform>(player);
    auto sprite = game->getECSManager().getComponent<sf::Sprite>(player);

    processActionList(*transform);
    updatePosition(*transform, 1.f);

    int textureWidth = sprite->getTexture()->getSize().x;
    int textureHeight = sprite->getTexture()->getSize().y;

    auto& position = transform->position;
    if (position.x < -textureWidth)
    {
        position.x = game->getWidth();
    }
    else if (position.x > game->getWidth())
    {
        position.x = 1 - textureWidth;
    }
    else if (position.y < -textureHeight)
    {
        position.y = game->getHeight();
    }
    else if (position.y > game->getHeight())
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
    auto transform = game->getECSManager().getComponent<Transform>(asteroid);
    auto sprite = game->getECSManager().getComponent<sf::Sprite>(asteroid);

    updatePosition(*transform, 1.f);

    int textureWidth = sprite->getTexture()->getSize().x;
    int textureHeight = sprite->getTexture()->getSize().y;
    int borderMargin = textureWidth * 2;
    auto& position = transform->position;
    if (position.x < -borderMargin || position.x > game->getWidth() + borderMargin ||
        position.y < -borderMargin || position.y > game->getHeight() + borderMargin)
    {
        asteroidCount--;
        game->getECSManager().removeEntity(asteroid);

        createAsteroid();
    }
}

void SceneAsteroids::updateLaser(Entity laser)
{
    auto transform = game->getECSManager().getComponent<Transform>(laser);
    auto sprite = game->getECSManager().getComponent<sf::Sprite>(laser);

    updatePosition(*transform, transform->speed);

    int textureWidth = sprite->getTexture()->getSize().x;
    int textureHeight = sprite->getTexture()->getSize().y;
    int borderMargin = textureWidth * 2;
    auto& position = transform->position;
    if (position.x < -borderMargin || position.x > game->getWidth() + borderMargin ||
        position.y < -borderMargin || position.y > game->getHeight() + borderMargin)
    {
        game->getECSManager().removeEntity(laser);
    }
}

void SceneAsteroids::processActionList(Transform& transform)
{
    auto actionList = game->getECSManager().getAnyComponent<std::vector<Action>>();

    float rotationSpeed = 6.f;

    for (auto& action : *actionList)
    {
        switch (action)
        {
            case THROTTLE:
            {
                if (transform.speed < maxPlayerSpeed)
                {
                    transform.speed += playerSpeedIncrement;
                    if (transform.speed > maxPlayerSpeed) transform.speed = maxPlayerSpeed;
                }
                break;
            }
            case ROTATE_LEFT:
            {
                transform.degrees -= rotationSpeed;
                if (transform.degrees < 0)
                {
                    transform.degrees = 360 - transform.degrees;
                }
                break;
            }
            case ROTATE_RIGHT:
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
    auto player = getEntityWithTag(PLAYER);
    auto playerTransform = game->getECSManager().getComponent<Transform>(player);
    auto playerBoundingBox = game->getECSManager().getComponent<BoundingBox>(player);

    auto asteroids = getEntitiesWithTag(ASTEROID);
    for (auto asteroid : asteroids)
    {
        auto asteroidTransform = game->getECSManager().getComponent<Transform>(asteroid);
        auto asteroidBoundingBox = game->getECSManager().getComponent<BoundingBox>(asteroid);

        Vec2 deltaV = playerTransform->position - asteroidTransform->position;
        float length = deltaV.length();

        if (length <= playerBoundingBox->radius || length <= asteroidBoundingBox->radius)
        {
            std::cout << "Hit detected!" << std::endl;
            auto playerStats = game->getECSManager().getComponent<PlayerStats>(player);
            playerStats->alive = false;
            createExplosion(playerTransform->position);
        }
    }
}

void SceneAsteroids::detectLaserCollision(Entity laser)
{
    auto laserTransform = game->getECSManager().getComponent<Transform>(laser);
    auto laserBoundingBox = game->getECSManager().getComponent<BoundingBox>(laser);

    auto asteroids = getEntitiesWithTag(ASTEROID);
    for (auto asteroid : asteroids)
    {
        auto asteroidTransform = game->getECSManager().getComponent<Transform>(asteroid);
        auto asteroidBoundingBox = game->getECSManager().getComponent<BoundingBox>(asteroid);

        Vec2 deltaV = laserTransform->position - asteroidTransform->position;
        float length = deltaV.length();

        if (length <= laserBoundingBox->radius || length <= asteroidBoundingBox->radius)
        {
            game->getECSManager().removeEntity(asteroid);
            asteroidCount--;
            createExplosion(asteroidTransform->position);

            game->getECSManager().removeEntity(laser);
            return;
        }
    }
}

void SceneAsteroids::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();

    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == BACKGROUND)
        {
            if (drawTextures)
            {
                auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);
                window.draw(*sprite);
            }
        }
    }

    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == ASTEROID)
        {
            auto transform = game->getECSManager().getComponent<Transform>(entity);

            if (drawTextures)
            {
                auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);

                sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
                sprite->setOrigin(size.x / 2.f, size.y / 2.f);
                sprite->rotate(1.f);
                sprite->setPosition(transform->position.x, transform->position.y);
                sprite->setScale(transform->scale, transform->scale);

                window.draw(*sprite);
            }

            if (drawCollision)
            {
                auto boundingBox = game->getECSManager().getComponent<BoundingBox>(entity);

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

    auto lasers = getEntitiesWithTag(LASER);
    for (auto laser : lasers)
    {
        auto transform = game->getECSManager().getComponent<Transform>(laser);
        if (drawTextures)
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(laser);

            sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
            sprite->setOrigin(size.x / 2.0f, size.y / 2.0f);
            sprite->setPosition(transform->position.x, transform->position.y);
            sprite->setRotation(transform->degrees - 90.f);
            sprite->setScale(transform->scale, transform->scale);

            window.draw(*sprite);
        }

        if (drawCollision)
        {
            auto boundingBox = game->getECSManager().getComponent<BoundingBox>(laser);

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

    auto explosions = getEntitiesWithTag(EXPLOSION);
    for (auto explosion : explosions)
    {
        if (drawTextures)
        {
            auto explTransform = game->getECSManager().getComponent<Transform>(explosion);
            auto explosionAnimation = game->getECSManager().getComponent<Animation>(explosion);
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(explosion);
            explosionAnimation->sprite = sprite.get();
            animationController.updateAnimationFrame(*explosionAnimation);

            explosionAnimation->sprite->setOrigin(explosionAnimation->width / 2.0f, explosionAnimation->height / 2.0f);
            explosionAnimation->sprite->setPosition(explTransform->position.x, explTransform->position.y);
            explosionAnimation->sprite->setRotation(explTransform->degrees);
            explosionAnimation->sprite->setScale(explTransform->scale, explTransform->scale);

            if (explosionAnimation->finished)
            {
                game->getECSManager().removeEntity(explosion);
            }
            else
            {
                window.draw(*sprite);
            }

            window.draw(*sprite);
        }
    }

    auto player = getEntityWithTag(PLAYER);
    auto playerStats = game->getECSManager().getComponent<PlayerStats>(player);
    if (playerStats->alive)
    {
        auto playerTransform = game->getECSManager().getComponent<Transform>(player);

        if (drawTextures)
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(player);

            sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
            sprite->setOrigin(size.x / 2.0f, size.y / 2.0f);
            sprite->setPosition(playerTransform->position.x, playerTransform->position.y);
            sprite->setRotation(playerTransform->degrees - 90.f);
            sprite->setScale(playerTransform->scale, playerTransform->scale);

            window.draw(*sprite);
        }

        if (drawCollision)
        {
            auto boundingBox = game->getECSManager().getComponent<BoundingBox>(player);

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