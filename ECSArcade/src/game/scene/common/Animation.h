#pragma once
#include <SFML/Graphics.hpp>

struct Animation {
    int rows;       // Number of rows in the spritesheet
    int columns;    // Number of columns in the spritesheet
    int width;      // Width of each frame
    int height;     // Height of each frame
    int index;      // Current frame index
    int totalFrames = 0;
    sf::Sprite* sprite; // Pointer to the sprite object

    int cooldown = 4;
    int currentCooldown = 0;
    sf::Color alphaColor; // Color to be made transparent
    bool loopAnimation = false;
    bool finished = false;
};

class AnimationController
{
public:
    AnimationController();
    void updateAnimationFrame(Animation& animation);
};