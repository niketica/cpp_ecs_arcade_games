#include "SceneAsteroids.h"

SceneAsteroids::SceneAsteroids(Game* game) : Scene(game) 
{
    init();
}

void SceneAsteroids::init()
{
    std::cout << "SceneAsteroids - init" << std::endl;

    game->getECSManager().reset();

    createActionList();
    createPlayer();
    createBackground();
    createAsteroid();
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
}

void SceneAsteroids::createAsteroid()
{
    Entity entity = game->getECSManager().addEntity();
    EntityTag entityTag;
    entityTag.value = ASTEROID;
    game->getECSManager().addComponent<EntityTag>(entity, entityTag);

    Transform transform;
    transform.position = { 250, 320 };
    transform.velocity = { 0, 0 };
    transform.speed = 2.f;
    transform.degrees = 90.f;
    game->getECSManager().addComponent<Transform>(entity, transform);

    sf::Sprite sprite;
    sprite = game->getAssetManager().getSprite("asteroid");
    game->getECSManager().addComponent<sf::Sprite>(entity, sprite);

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
            if (keyInput->keyType == S)
            {

            }
            if (keyInput->keyType == A)
            {
                if (!(std::find(actionList->begin(), actionList->end(), ROTATE_LEFT) != actionList->end()))
                {
                    actionList->push_back(ROTATE_LEFT);
                }
            }
            if (keyInput->keyType == D)
            {
                if (!(std::find(actionList->begin(), actionList->end(), ROTATE_RIGHT) != actionList->end()))
                {
                    actionList->push_back(ROTATE_RIGHT);
                }
            }
            if (keyInput->keyType == ESCAPE_)
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
    updatePlayer();

    if (asteroidCount > 0)
    {
        updateAsteroid();
    }
}

Entity SceneAsteroids::getEntityWithTag(Tag tag)
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

    sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
    sprite->setOrigin(size / 2.f);
    sprite->setRotation(transform->degrees - 90);
    sprite->setPosition(position.x, position.y);

    if (transform->speed > 0)
    {
        transform->speed -= 0.05f;

        if (transform->speed < 0)
        {
            transform->speed = 0;
        }
    }
}

void SceneAsteroids::updateAsteroid()
{
    Entity asteroid = getEntityWithTag(ASTEROID);

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
    }

    sf::Vector2f size = (sf::Vector2f)sprite->getTexture()->getSize();
    sprite->setOrigin(size / 2.f);
    sprite->rotate(1.f);
    sprite->setPosition(position.x, position.y);
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

void SceneAsteroids::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();

    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == BACKGROUND)
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);
            window.draw(*sprite);
        }
    }

    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == ASTEROID)
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);
            window.draw(*sprite);
        }
    }

    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == PLAYER)
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);
            window.draw(*sprite);
        }
    }

    window.display();
}