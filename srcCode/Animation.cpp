#include <SFML/Graphics.hpp>
#include "Animation.h"

Animation::Animation (
    sf::Texture texture,
    int frameWidth,
    int frameHeight,
    int frameCount,
    int times,
    int row
) {
    this->texture = texture;
    this->frameWidth = frameWidth;
    this->frameHeight = frameHeight;
    this->frameCount = frameCount;
    this->times = times;
    this->row = row;

    this->currentFrame = 0;
    this->timer = 0;
}

void Animation::Update() {
    timer++;
    if (timer == times) {
        timer = 0;
        currentFrame++;

        if (currentFrame == frameCount) {
            isFinished = true;
            currentFrame = 0;
        }
    }
}

void Animation::Reset() {
    isFinished = false;
    currentFrame = 0;
    timer = 0;
}

bool Animation::isAnimationFinished() {
    return isFinished;
}

sf::IntRect Animation::getSrcRect() {
    return sf::IntRect (
        {currentFrame * frameWidth, row * frameHeight},
        {frameWidth, frameHeight}
    );
}