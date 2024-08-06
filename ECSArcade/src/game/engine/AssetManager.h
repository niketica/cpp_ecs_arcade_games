#pragma once

#include <map>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>

class AssetManager
{
public:
    void addFont(const char* name, const char* path);
    void addTexture(const char* name, const char* path);

    const sf::Font& getFont(const char* name) const;
    const sf::Texture& getTexture(const char* name) const;
    const sf::Sprite& getSprite(const char* name) const;

private:
    std::map<const char*, sf::Font> fontMap;
    std::map<const char*, sf::Texture> textureMap;
    std::map<const char*, sf::Sprite> spriteMap;

    void applyAlphaMask(sf::Texture& texture, const sf::Color& alphaColor) const;
};