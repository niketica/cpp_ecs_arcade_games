#include "AssetManager.h"

void AssetManager::addFont(const std::string& name, const std::string& path)
{
    std::cout << "Loading font - " << name << " - " << path << std::endl;

    fontMap[name] = sf::Font();
    if (!fontMap[name].loadFromFile(path))
    {
        std::cerr << "Could not load font!" << std::endl;
        exit(-1);
    }
}

const sf::Font& AssetManager::getFont(const std::string& name) const
{
    return fontMap.at(name);
}

void AssetManager::addTexture(const std::string& name, const std::string& path)
{
    std::cout << "Loading texture - " << name << " - " << path << std::endl;

    textureMap[name] = sf::Texture();
    if (!textureMap[name].loadFromFile(path))
    {
        std::cerr << "Could not load texture!" << std::endl;
        exit(-1);
    }

    spriteMap[name] = sf::Sprite();
    spriteMap[name].setTexture(textureMap[name]);
}

const sf::Texture& AssetManager::getTexture(const std::string& name) const
{
    return textureMap.at(name);
}

const sf::Sprite& AssetManager::getSprite(const std::string& name) const
{
    return spriteMap.at(name);
}