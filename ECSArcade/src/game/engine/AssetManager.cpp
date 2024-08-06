#include "AssetManager.h"

void AssetManager::addFont(const char* name, const char* path)
{
    std::cout << "Loading font - " << name << " - " << path << std::endl;

    fontMap[name] = sf::Font();
    if (!fontMap[name].loadFromFile(path))
    {
        std::cerr << "Could not load font!" << std::endl;
        exit(-1);
    }
}

const sf::Font& AssetManager::getFont(const char* name) const
{
    return fontMap.at(name);
}

void AssetManager::addTexture(const char* name, const char* path)
{
    std::cout << "Loading texture - " << name << " - " << path << std::endl;

    textureMap[name] = sf::Texture();
    if (!textureMap[name].loadFromFile(path))
    {
        std::cerr << "Could not load texture!" << std::endl;
        exit(-1);
    }

    // TODO alpha colors are specific for each image. Get these values from a config file
    sf::Color alphaColor_01(121, 230, 234);
    applyAlphaMask(textureMap[name], alphaColor_01);
    sf::Color alphaColor_02(55, 177, 182);
    applyAlphaMask(textureMap[name], alphaColor_02);

    spriteMap[name] = sf::Sprite();
    spriteMap[name].setTexture(textureMap[name]);
}

const sf::Texture& AssetManager::getTexture(const char* name) const
{
    return textureMap.at(name);
}

const sf::Sprite& AssetManager::getSprite(const char* name) const
{
    return spriteMap.at(name);
}

void AssetManager::applyAlphaMask(sf::Texture& texture, const sf::Color& alphaColor) const
{
    sf::Image image = texture.copyToImage();
    sf::Vector2u imageSize = image.getSize();

    for (unsigned int x = 0; x < imageSize.x; ++x) {
        for (unsigned int y = 0; y < imageSize.y; ++y) {
            if (image.getPixel(x, y) == alphaColor) {
                image.setPixel(x, y, sf::Color(0, 0, 0, 0)); // Set to fully transparent
            }
        }
    }

    texture.update(image);
}