#include "ScenePacman.h"

ScenePacman::ScenePacman(Game* game) : Scene(game) {}

void ScenePacman::init()
{
    createActionList();
}

void ScenePacman::createActionList()
{
    Entity eAction = getGame()->getECSManager().addEntity();
    std::vector<SnakeAction> actionList;
    getGame()->getECSManager().addComponent<std::vector<SnakeAction>>(eAction, actionList);
}

void ScenePacman::input()
{
    auto keyInputs = getGame()->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = getGame()->getECSManager().getAnyComponent<std::vector<SnakeAction>>();
            if (keyInput->keyType == W)
            {
                std::cout << "W pressed" << std::endl;
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
        }
    }
}

void ScenePacman::update(float deltaTime)
{

}

void ScenePacman::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    window.display();
}

