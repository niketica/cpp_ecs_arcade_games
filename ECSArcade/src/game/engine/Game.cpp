#include "Game.h"

Game::Game() : ecsManager(maxNrOfEntities) 
{
    init();
}

void Game::init()
{
    const int wWidth = 640;
    const int wHeight = 480;
    window.create(sf::VideoMode(wWidth, wHeight), "ECS Arcade");

    loadAssets();
    loadScene(std::make_shared<SceneTitleScreen>(this));
}

void Game::loadAssets()
{
    // External sources
    assetManager.addFont("OpenSans", "../External/assets/fonts/open-sans/OpenSans-Regular.ttf");
    assetManager.addTexture("explosion_realistic", "../External/assets/images/explosion_realistic_01.png");

    // Original sources
    // Title Screen
    assetManager.addTexture("title_background", "assets/images/title_background.png");

    // Asteroids
    assetManager.addTexture("space_ship", "assets/images/spaceship_01.png");
    assetManager.addTexture("space_background", "assets/images/space_background_02.png");
    assetManager.addTexture("asteroid", "assets/images/asteroid_01.png");
    assetManager.addTexture("laser", "assets/images/laser.png");
}

void Game::start()
{
    loop();
}

void Game::stop()
{
    running = false;

    if (window.isOpen())
    {
        window.close();
    }
}

void Game::loop()
{
    auto lastUpdate = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();

    float timeSlice = 1.f / 60.f;
    float accumulator = 0.f;

    int fps = 0;
    float fpsCount = 0.f;

    running = true;
    while (running)
    {
        now = std::chrono::system_clock::now();
        std::chrono::duration<float> deltaTime = now - lastUpdate;
        float fDeltaTime = deltaTime.count();
        lastUpdate = now;

        accumulator += fDeltaTime;

        input();
        while (accumulator > timeSlice)
        {
            update(timeSlice);
            accumulator -= timeSlice;
        }
        render();

        fpsCount += fDeltaTime;
        fps++;
        if (fpsCount >= 1.f)
        {
            fpsCount -= 1.f;
            std::cout << "FPS = " << fps << std::endl;
            fps = 0;
        }
    }
}

void Game::input()
{
    auto inputs = ecsManager.getEntitiesWithComponent<KeyInput>();
    for (auto prevInput : inputs)
    {
        ecsManager.removeEntity(prevInput);
    }

    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            stop();
        }

        if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
        {
            KeyType keyType;
            switch (event.key.code)
            {
            case sf::Keyboard::W:
                keyType = W;
                break;
            case sf::Keyboard::A:
                keyType = A;
                break;
            case sf::Keyboard::S:
                keyType = S;
                break;
            case sf::Keyboard::D:
                keyType = D;
                break;
            case sf::Keyboard::C:
                keyType = C;
                break;
            case sf::Keyboard::R:
                keyType = R;
                break;
            case sf::Keyboard::T:
                keyType = T;
                break;
            case sf::Keyboard::Space:
                keyType = SPACE_KEY;
                break;
            case sf::Keyboard::Escape:
                keyType = ESCAPE_KEY;
                break;
            default:
                keyType = UNDEFINED_KEY;
                break;
            }

            if (keyType != UNDEFINED_KEY)
            {
                KeyInput keyInput{};
                keyInput.inputType = event.type == sf::Event::KeyPressed ? PRESSED : RELEASED;
                keyInput.keyType = keyType;
                Entity keyEvent = ecsManager.addEntity();

                ecsManager.addComponent<KeyInput>(keyEvent, keyInput);
            }
        }
    }

    currentScene->input();
}

void Game::update(float deltaTime)
{
    currentScene->update(deltaTime);
}

void Game::render()
{
    currentScene->render(window);
}

ECSManager& Game::getECSManager()
{
    return ecsManager;
}

AssetManager& Game::getAssetManager()
{
    return assetManager;
}

int Game::getWidth() const
{
    return (int) window.getView().getSize().x;
}

int Game::getHeight() const
{
    return (int) window.getView().getSize().y;
}

void Game::loadScene(std::shared_ptr<Scene> scene)
{
    currentScene = scene;
    currentScene->init();
}

Entity Game::getEntityWithTag(int tag)
{
    auto entities = getECSManager().getEntitiesWithComponent<EntityTag>();

    Entity returnEntity = NULL;
    for (auto entity : entities)
    {
        auto entityTag = getECSManager().getComponent<EntityTag>(entity);

        if (entityTag->value == tag)
        {
            returnEntity = entity;
        }
    }

    if (returnEntity == NULL)
    {
        std::cerr << "ERROR!!! Entity not found with tag " << tag << std::endl;
        exit(-1);
    }

    return returnEntity;
}

std::vector<Entity> Game::getEntitiesWithTag(int tag)
{
    auto entities = getECSManager().getEntitiesWithComponent<EntityTag>();
    std::vector<Entity> filtered;

    for (auto entity : entities)
    {
        auto entityTag = getECSManager().getComponent<EntityTag>(entity);
        if (entityTag->value == tag)
        {
            filtered.push_back(entity);
        }
    }

    return filtered;
}