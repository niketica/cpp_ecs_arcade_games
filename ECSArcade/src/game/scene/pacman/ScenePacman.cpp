#include "ScenePacman.h"

ScenePacman::ScenePacman(Game* game) : Scene(game)
{
    init();
}

void ScenePacman::init()
{
    createActionList();
}

void ScenePacman::createActionList()
{
    Entity eAction = game->getECSManager().addEntity();
    std::vector<SnakeAction> actionList;
    game->getECSManager().addComponent<std::vector<SnakeAction>>(eAction, actionList);
}

void ScenePacman::input()
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

void ScenePacman::update()
{

}

void ScenePacman::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    window.display();
}

