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
    window.setFramerateLimit(60);

    loadAssets();

    currentScene = std::make_shared<SceneTitleScreen>(this);
}

void Game::loadAssets()
{
    assetManager.addFont("OpenSans", "../External/assets/fonts/open-sans/OpenSans-Regular.ttf");
    assetManager.addTexture("space_ship", "assets/images/spaceship_01.png");
    assetManager.addTexture("space_background", "assets/images/space_background_02.png");
    assetManager.addTexture("asteroid", "assets/images/asteroid_01.png");
}

void Game::start()
{
    Entity e = ecsManager.addEntity();
    auto gameStatus = GameStatus(true);
    ecsManager.addComponent<GameStatus>(e, gameStatus);

    loop();
}

void Game::stop()
{
    auto gameStatus = ecsManager.getAnyComponent<GameStatus>();
    gameStatus->running = false;

    if (window.isOpen())
    {
        window.close();
    }
}

void Game::loop()
{
    auto gameStatus = ecsManager.getAnyComponent<GameStatus>();
    while (gameStatus->running)
    {
        input();
        update();
        render();
    }
}

void Game::input()
{
    auto inputs = ecsManager.getEntitiesWithComponent<KeyInput>();
    // std::cout << "inputs size = " << inputs.size() << std::endl;
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
                KeyInput keyInput;
                keyInput.inputType = event.type == sf::Event::KeyPressed ? PRESSED : RELEASED;
                keyInput.keyType = keyType;
                Entity keyEvent = ecsManager.addEntity();

                ecsManager.addComponent<KeyInput>(keyEvent, keyInput);
            }
        }
    }

    currentScene->input();
}

void Game::update()
{
    currentScene->update();
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
}