#include "SceneSnake.h"

SceneSnake::SceneSnake(Game* game) : Scene(game)
{
    init();
}

void SceneSnake::init()
{
    createActionList();
}

void SceneSnake::createActionList()
{
    Entity eAction = game->getECSManager().addEntity();
    std::vector<SnakeAction> actionList;
    game->getECSManager().addComponent<std::vector<SnakeAction>>(eAction, actionList);
}

void SceneSnake::input()
{
    auto keyInputs = game->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = game->getECSManager().getAnyComponent<std::vector<SnakeAction>>();
            if (keyInput->keyType == W)
            {
                std::cout << "W pressed" << std::endl;
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
        }
    }
}

void SceneSnake::update()
{

}

void SceneSnake::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    window.display();
}

