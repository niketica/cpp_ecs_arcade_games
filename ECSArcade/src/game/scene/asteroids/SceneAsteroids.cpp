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
}

void SceneAsteroids::createPlayer()
{
    Entity ePlayer = game->getECSManager().addEntity();
    EntityTag entityTag;
    entityTag.tag = PLAYER;
    game->getECSManager().addComponent<EntityTag>(ePlayer, entityTag);

    Player cPlayer;
    cPlayer.position = { 100, 200 };
    cPlayer.velocity = { 0, 0 };
    game->getECSManager().addComponent<Player>(ePlayer, cPlayer);

    sf::Sprite playerSprite;
    playerSprite = game->getAssetManager().getSprite("space_ship");
    game->getECSManager().addComponent<sf::Sprite>(ePlayer, playerSprite);
}

void SceneAsteroids::createBackground()
{
    Entity eBackground = game->getECSManager().addEntity();
    EntityTag backgroundTag;
    backgroundTag.tag = BACKGROUND;
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
    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();

    Entity ePlayer = NULL;
    for (auto entity : entities)
    {
        auto entityTag = game->getECSManager().getComponent<EntityTag>(entity);

        if (entityTag->tag == PLAYER)
        {
            ePlayer = entity;
        }
    }

    if (ePlayer == NULL)
    {
        std::cerr << "ERROR!!! Player entity not found" << std::endl;
        game->getECSManager().printComponents();
        exit(-1);
    }

    auto cPlayer = game->getECSManager().getComponent<Player>(ePlayer);
    auto cPlayerSprite = game->getECSManager().getComponent<sf::Sprite>(ePlayer);

    processActionList(*cPlayer);
    updatePlayerPosition(*cPlayer, 1.f);
    
    sf::Vector2f size = (sf::Vector2f) cPlayerSprite->getTexture()->getSize();
    cPlayerSprite->setOrigin(size / 2.f);
    cPlayerSprite->setRotation(cPlayer->degrees - 90);

    int textureWidth = cPlayerSprite->getTexture()->getSize().x;
    int textureHeight = cPlayerSprite->getTexture()->getSize().y;

    auto& position = cPlayer->position;
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

    cPlayerSprite->setPosition(position.x, position.y);

    if (cPlayer->speed > 0)
    {
        cPlayer->speed -= 0.05f;

        if (cPlayer->speed < 0)
        {
            cPlayer->speed = 0;
        }
    }
}

void SceneAsteroids::processActionList(Player& cPlayer)
{
    auto actionList = game->getECSManager().getAnyComponent<std::vector<Action>>();

    float rotationSpeed = 6.f;

    for (auto& action : *actionList)
    {
        switch (action)
        {
            case THROTTLE:
            {
                if (cPlayer.speed < maxPlayerSpeed)
                {
                    cPlayer.speed += playerSpeedIncrement;
                    if (cPlayer.speed > maxPlayerSpeed) cPlayer.speed = maxPlayerSpeed;
                }
                break;
            }
            case ROTATE_LEFT:
            {
                cPlayer.degrees -= rotationSpeed;
                if (cPlayer.degrees < 0)
                {
                    cPlayer.degrees = 360 - cPlayer.degrees;
                }
                break;
            }
            case ROTATE_RIGHT:
            {
                cPlayer.degrees += rotationSpeed;
                if (cPlayer.degrees > 360)
                {
                    cPlayer.degrees = cPlayer.degrees - 360;
                }
                break;
            }
        }
    }
}

void SceneAsteroids::updatePlayerPosition(Player& player, float deltaTime) {
    float radians = player.degrees * (M_PI / 180.0f);

    player.velocity.x = std::cos(radians) * player.speed;
    player.velocity.y = std::sin(radians) * player.speed;
    player.position = player.position + (player.velocity * deltaTime);
}

void SceneAsteroids::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();

    Entity player = NULL;
    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->tag == PLAYER)
        {
            // draw player last so player is always visible
            // too lazy to do sprite layers in this project
            player = entity;
        }
        else
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);
            window.draw(*sprite);
        }
    }

    if (player == NULL)
    {
        std::cerr << "Player entity not found!" << std::endl;
        exit(-1);
    }
    else
    {
        auto sprite = game->getECSManager().getComponent<sf::Sprite>(player);
        window.draw(*sprite);
    }

    window.display();
}