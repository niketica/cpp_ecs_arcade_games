#pragma once

#include <map>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

class AssetManager
{
public:
    void addFont(const std::string& name, const std::string& path);
    void addTexture(const std::string& name, const std::string& path);

    const sf::Font& getFont(const std::string& name) const;
    const sf::Texture& getTexture(const std::string& name) const;
    const sf::Sprite& getSprite(const std::string& name) const;

private:
    std::map<std::string, sf::Font> fontMap;
    std::map<std::string, sf::Texture> textureMap;
    std::map<std::string, sf::Sprite> spriteMap;
};