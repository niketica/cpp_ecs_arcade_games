#pragma once

#include "memory"
#include "SFML/Graphics.hpp"
#include "../../ecs/ECSManager.h"
#include "AssetManager.h"
#include "../scene/common/Scene.h"
#include "../scene/titlescreen/SceneTitleScreen.h"

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
    int value = -1;
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
    const Entity maxNrOfEntities = 100;
    ECSManager ecsManager;
    AssetManager assetManager;
    sf::RenderWindow window;
    std::shared_ptr<Scene> currentScene = nullptr;

    void init();
    void loadAssets();
    void loop();
    void input();
    void update();
    void render();
};