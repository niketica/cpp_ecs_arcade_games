#include "SceneTitleScreen.h"

SceneTitleScreen::SceneTitleScreen(Game* game) : Scene(game) {}

void SceneTitleScreen::init()
{
    getGame()->getECSManager().reset();
    createTitle();
    createMenuItems();
    createBackground();
}

void SceneTitleScreen::createTitle()
{
    auto& openSans = getGame()->getAssetManager().getFont("OpenSans");
    sf::Text titleText("ECS Arcade", openSans, 48);

    auto xPos = (getGame()->getWidth() / 2) - (titleText.getLocalBounds().width / 2);
    auto yPos = 0.f;
    titleText.setPosition(xPos, yPos);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(titleColor);

    Title title;
    title.text = titleText;
    Entity eTitleText = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<Title>(eTitleText, title);
}

void SceneTitleScreen::createMenuItems()
{
    auto& openSans = getGame()->getAssetManager().getFont("OpenSans");
    std::vector<std::string> menuOptions;
    menuOptions.push_back("ASTEROIDS");
    menuOptions.push_back("SNAKE");
    menuOptions.push_back("TETRIS");
    menuOptions.push_back("PACMAN");
    menuOptions.push_back("EXIT");

    int verticalSpacing = 0;
    int index = 0;
    for (int index = 0; index < menuOptions.size(); index++)
    {
        std::string strMenuOption = menuOptions.at(index);
        sf::Text menuOption(strMenuOption, openSans, 30);

        auto xPos = (getGame()->getWidth() / 2) - (menuOption.getLocalBounds().width / 2);
        auto yPos = 100.f + verticalSpacing;
        menuOption.setPosition(xPos, yPos);
        menuOption.setStyle(sf::Text::Bold);

        Entity eMenuOption = getGame()->getECSManager().addEntity();

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

        getGame()->getECSManager().addComponent<MenuItem>(eMenuOption, menuItem);

        verticalSpacing += menuOption.getCharacterSize();
    }
}

void SceneTitleScreen::createBackground()
{
    EntityTag backgroundTag;
    backgroundTag.value = TITLE_BACKGROUND;

    sf::Sprite backgroundSprite;
    backgroundSprite = getGame()->getAssetManager().getSprite("title_background");
    backgroundSprite.setScale(0.5f, 0.5f);

    Entity eBackground = getGame()->getECSManager().addEntity();
    getGame()->getECSManager().addComponent<EntityTag>(eBackground, backgroundTag);
    getGame()->getECSManager().addComponent<sf::Sprite>(eBackground, backgroundSprite);
}

void SceneTitleScreen::input()
{
    auto keyInputs = getGame()->getECSManager().getComponents<KeyInput>();
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
                auto menuItems = getGame()->getECSManager().getComponents<MenuItem>();
                for (auto& menuItem : menuItems)
                {
                    if (!menuItem->active) continue;
                    switch (menuItem->index)
                    {
                    case 0:
                        getGame()->loadScene(std::make_shared<SceneAsteroids>(getGame()));
                        break;
                    case 1:
                        getGame()->loadScene(std::make_shared<SceneSnake>(getGame()));
                        break;
                    case 2:
                        getGame()->loadScene(std::make_shared<SceneTetris>(getGame()));
                        break;
                    case 3:
                        getGame()->loadScene(std::make_shared<ScenePacman>(getGame()));
                        break;
                    case 4:
                        getGame()->stop();
                        break;
                    }
                }
            }
            else if (keyInput->keyType == ESCAPE_KEY)
            {
                getGame()->stop();
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

void SceneTitleScreen::update(float deltaTime)
{
}

int SceneTitleScreen::getNextActiveMenuItemIndex()
{
    auto menuItems = getGame()->getECSManager().getComponents<MenuItem>();
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
    auto menuItems = getGame()->getECSManager().getComponents<MenuItem>();
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
    auto menuItems = getGame()->getECSManager().getComponents<MenuItem>();
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

    auto entities = getGame()->getECSManager().getEntitiesWithComponent<EntityTag>();
    for (auto& entity : entities)
    {
        auto tag = getGame()->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == TITLE_BACKGROUND)
        {
            auto sprite = getGame()->getECSManager().getComponent<sf::Sprite>(entity);
            window.draw(*sprite);
        }
    }

    auto menuItems = getGame()->getECSManager().getComponents<MenuItem>();
    for (auto& menuItem : menuItems)
    {
        window.draw(menuItem->text);
    }
    
    auto titles = getGame()->getECSManager().getComponents<Title>();
    for (auto& title : titles)
    {
        window.draw(title->text);
    }

    window.display();
}