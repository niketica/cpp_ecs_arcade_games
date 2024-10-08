#pragma once

#include <memory>
#include <chrono>
#include <SFML/Graphics.hpp>

#include "ecs/ECSManager.h"
#include "game/engine/AssetManager.h"
#include "game/scene/common/Scene.h"
#include "game/scene/titlescreen/SceneTitleScreen.h"

enum KeyType
{
    UNDEFINED_KEY,
    W,
    A,
    S,
    D,
    C,
    R,
    T,
    SPACE_KEY,
    ESCAPE_KEY,
};

enum InputType
{
    PRESSED,
    RELEASED
};

struct KeyInput
{
    KeyType keyType;
    InputType inputType;
};

struct EntityTag
{
    EntityTag() = default;
    explicit EntityTag(int value) : value(value) {}

    int value = -1;
};

struct GameStatus
{
    bool running = false;
};

class Game
{
public:
    Game();

    void start();
    void stop();
    void loadScene(std::shared_ptr<Scene> scene);

    ECSManager& getECSManager();
    AssetManager& getAssetManager();
    int getWidth() const;
    int getHeight() const;

    Entity getEntityWithTag(int tag);
    std::vector<Entity> getEntitiesWithTag(int tag);

private:
    const Entity maxNrOfEntities = 5000;
    ECSManager ecsManager;
    AssetManager assetManager;
    sf::RenderWindow window;
    std::shared_ptr<Scene> currentScene = nullptr;
    bool running = false;

    void init();
    void loadAssets();
    void loop();
    void input();
    void update(float deltaTime);
    void render();
};