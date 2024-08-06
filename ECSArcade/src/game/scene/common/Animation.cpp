#include "Animation.h"

void AnimationController::updateAnimation(Animation& animation, float deltaTime) const {
    animation.currentFrameTime += deltaTime;
    if (animation.currentFrameTime >= animation.frameTime)
    {
        updateAnimationIndex(animation);
        updateAnimationFrame(animation);
        animation.currentFrameTime -= animation.frameTime;
    }
}

void AnimationController::updateAnimationIndex(Animation& animation) const
{
    if (animation.loopAnimation)
    {
        animation.index = (animation.index + 1) % animation.totalFrames;
    }
    else
    {
        animation.index++;
        if (animation.index >= animation.totalFrames)
        {
            animation.finished = true;
        }
    }
}

void AnimationController::updateAnimationFrame(Animation& animation) const
{
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