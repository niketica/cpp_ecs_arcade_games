#include "SceneTitleScreen.h"

SceneTitleScreen::SceneTitleScreen(Game* game) : Scene(game) 
{
    init();
}

void SceneTitleScreen::init()
{
    game->getECSManager().reset();
    createTitle();
    createMenuItems();
    createBackground();
}

void SceneTitleScreen::createTitle()
{
    auto& openSans = game->getAssetManager().getFont("OpenSans");
    sf::Text titleText("ECS Arcade", openSans, 48);

    auto xPos = (game->getWidth() / 2) - (titleText.getLocalBounds().width / 2);
    auto yPos = 0.f;
    titleText.setPosition(xPos, yPos);
    titleText.setStyle(sf::Text::Bold);
    titleText.setFillColor(titleColor);

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
        sf::Text menuOption(strMenuOption, openSans, 30);

        auto xPos = (game->getWidth() / 2) - (menuOption.getLocalBounds().width / 2);
        auto yPos = 100.f + verticalSpacing;
        menuOption.setPosition(xPos, yPos);
        menuOption.setStyle(sf::Text::Bold);

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

void SceneTitleScreen::createBackground()
{
    EntityTag backgroundTag;
    backgroundTag.value = TITLE_BACKGROUND;

    sf::Sprite backgroundSprite;
    backgroundSprite = game->getAssetManager().getSprite("title_background");
    backgroundSprite.setScale(0.5f, 0.5f);

    Entity eBackground = game->getECSManager().addEntity();
    game->getECSManager().addComponent<EntityTag>(eBackground, backgroundTag);
    game->getECSManager().addComponent<sf::Sprite>(eBackground, backgroundSprite);
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

    auto entities = game->getECSManager().getEntitiesWithComponent<EntityTag>();
    for (auto& entity : entities)
    {
        auto tag = game->getECSManager().getComponent<EntityTag>(entity);
        if (tag->value == TITLE_BACKGROUND)
        {
            auto sprite = game->getECSManager().getComponent<sf::Sprite>(entity);
            window.draw(*sprite);
        }
    }

    auto menuItems = game->getECSManager().getComponents<MenuItem>();
    for (auto& menuItem : menuItems)
    {
        window.draw(menuItem->text);
    }
    
    auto title = game->getECSManager().getAnyComponent<Title>();
    window.draw(title->text);

    window.display();
}