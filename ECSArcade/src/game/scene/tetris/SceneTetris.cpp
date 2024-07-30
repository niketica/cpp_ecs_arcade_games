#include "SceneTetris.h"

SceneTetris::SceneTetris(Game* game) : Scene(game)
{
    init();
}

void SceneTetris::init()
{
    createActionList();
}

void SceneTetris::createActionList()
{
    Entity eAction = game->getECSManager().addEntity();
    std::vector<TetrisAction> actionList;
    game->getECSManager().addComponent<std::vector<TetrisAction>>(eAction, actionList);
}

void SceneTetris::input()
{
    auto keyInputs = game->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            auto actionList = game->getECSManager().getAnyComponent<std::vector<TetrisAction>>();
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

void SceneTetris::update()
{

}

void SceneTetris::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    window.display();
}

