#include "Animation.h"


AnimationController::AnimationController() {}

void AnimationController::updateAnimationFrame(Animation& animation) {
    if (animation.currentCooldown == 0)
    {
        // Advance to the next frame
        animation.index = (animation.index + 1) % animation.totalFrames;
    }
    animation.currentCooldown++;
    if (animation.currentCooldown >= animation.cooldown)
    {
        animation.currentCooldown = 0;
    }

    // Calculate the row and column for the current frame
    int row = animation.index / animation.columns;
    int column = animation.index % animation.columns;

    // Set the texture rect to display the correct frame
    sf::IntRect frameRect(
        column * animation.width,  // x position
        row * animation.height,    // y position
        animation.width,           // width of the frame
        animation.height           // height of the frame
    );

    // Apply the texture rect to the sprite
    animation.sprite->setTextureRect(frameRect);
}