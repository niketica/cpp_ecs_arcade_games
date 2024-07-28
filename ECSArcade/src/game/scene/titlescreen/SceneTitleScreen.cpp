#include "SceneTitleScreen.h"

SceneTitleScreen::SceneTitleScreen(Game* game) : Scene(game) 
{
    init();
}

void SceneTitleScreen::init()
{
    std::cout << "SceneTitleScreen - init" << std::endl;
    game->getECSManager().reset();
    createTitle();
    createMenuItems();
}

void SceneTitleScreen::createTitle()
{
    auto& openSans = game->getAssetManager().getFont("OpenSans");
    sf::Text titleText("ECS Arcade", openSans, 48);
    titleText.setPosition(0, 0);
    Title title;
    title.text = titleText;
    Entity eTitleText = game->getECSManager().addEntity();
    game->getECSManager().addComponent<Title>(eTitleText, title);
}

void SceneTitleScreen::createMenuItems()
{
    auto& openSans = game->getAssetManager().getFont("OpenSans");
    std::vector<std::string> menuOptions;
    menuOptions.push_back("Asteroids");
    menuOptions.push_back("Snake");
    menuOptions.push_back("Tetris");
    menuOptions.push_back("Pacman");

    int verticalSpacing = 0;
    int index = 0;
    for (int index = 0; index < menuOptions.size(); index++)
    {
        std::string strMenuOption = menuOptions.at(index);
        sf::Text menuOption(strMenuOption, openSans, 24);
        menuOption.setPosition(0.f, 100.f + verticalSpacing);
        Entity eMenuOption = game->getECSManager().addEntity();

        MenuItem menuItem;
        menuItem.index = index;
        menuItem.text = menuOption;

        if (index == 0)
        {
            menuItem.active = true;
            menuItem.text.setFillColor(activeMenuItemColor);
        }
        else
        {
            menuItem.text.setFillColor(inactiveMenuItemColor);
        }

        game->getECSManager().addComponent<MenuItem>(eMenuOption, menuItem);

        verticalSpacing += menuOption.getCharacterSize();
    }
}

void SceneTitleScreen::input()
{
    auto keyInputs = game->getECSManager().getComponents<KeyInput>();
    for (auto& keyInput : keyInputs)
    {
        if (keyInput->inputType == PRESSED)
        {
            if (keyInput->keyType == W)
            {
                int activeIndex = getPreviousActiveMenuItemIndex();
                updateMenuItems(activeIndex);
            }
            else if (keyInput->keyType == S)
            {
                int activeIndex = getNextActiveMenuItemIndex();
                updateMenuItems(activeIndex);
            }
            else if (keyInput->keyType == D)
            {
                auto menuItems = game->getECSManager().getComponents<MenuItem>();
                for (auto& menuItem : menuItems)
                {
                    if (menuItem->index == 0 && menuItem->active)
                    {
                        game->loadScene(std::make_shared<SceneAsteroids>(game));
                    }
                }
            }
            else if (keyInput->keyType == ESCAPE_KEY)
            {
                game->stop();
            }
        }
        else if (keyInput->inputType == RELEASED)
        {
            if (keyInput->keyType == W)
            {
                // std::cout << "W released" << std::endl;
            }
        }
    }
}

void SceneTitleScreen::update()
{
}

int SceneTitleScreen::getNextActiveMenuItemIndex()
{
    auto menuItems = game->getECSManager().getComponents<MenuItem>();
    int activeIndex = -1;
    for (auto& menuItem : menuItems)
    {
        if (menuItem->active)
        {
            activeIndex = menuItem->index;
        }
    }
    activeIndex++;
    if (activeIndex >= menuItems.size())
    {
        activeIndex = 0;
    }
    return activeIndex;
}

int SceneTitleScreen::getPreviousActiveMenuItemIndex()
{
    auto menuItems = game->getECSManager().getComponents<MenuItem>();
    size_t activeIndex = 0;
    for (auto& menuItem : menuItems)
    {
        if (menuItem->active)
        {
            activeIndex = menuItem->index;
        }
    }
    if (activeIndex == 0)
    {
        activeIndex = menuItems.size() - 1;
    }
    else
    {
        activeIndex--;
    }
    return (int) activeIndex;
}

void SceneTitleScreen::updateMenuItems(int activeIndex)
{
    auto menuItems = game->getECSManager().getComponents<MenuItem>();
    for (int index = 0; index < menuItems.size(); index++)
    {
        auto& menuItem = menuItems.at(index);
        if (index == activeIndex)
        {
            menuItem->active = true;
            menuItem->text.setFillColor(activeMenuItemColor);
        }
        else
        {
            menuItem->active = false;
            menuItem->text.setFillColor(inactiveMenuItemColor);
        }
    }
}

void SceneTitleScreen::render(sf::RenderWindow& window)
{
    window.clear(windowClearColor);

    auto menuItems = game->getECSManager().getComponents<MenuItem>();
    for (auto& menuItem : menuItems)
    {
        window.draw(menuItem->text);
    }
    
    auto title = game->getECSManager().getAnyComponent<Title>();
    window.draw(title->text);

    window.display();
}