#pragma once

#include <iostream>
#include <SFML/Graphics.hpp>

struct Animation {
    int rows;       // Number of rows in the spritesheet
    int columns;    // Number of columns in the spritesheet
    int width;      // Width of each frame
    int height;     // Height of each frame
    int index = 0;      // Current frame index
    int totalFrames = 0;
    sf::Sprite* sprite; // Pointer to the sprite object

    sf::Color alphaColor; // Color to be made transparent
    bool loopAnimation = false;
    bool finished = false;
    float frameTime = 1.f / 30.f; // Animations are 30fps by default
    float currentFrameTime = 0.f;
};

class AnimationController
{
public:
    AnimationController() = default;
    void updateAnimation(Animation& animation, float deltaTime) const;
    void updateAnimationIndex(Animation& animation) const;
    void updateAnimationFrame(Animation& animation) const;
};