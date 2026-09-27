#ifndef ANIMATION
#define ANIMATION

#include <SFML/Graphics.hpp>

class Animation {
    private:
        sf::Texture texture;
        int frameHeight;
        int frameWidth;
        int frameCount;
        int times;
        int row; 

        int currentFrame;
        int timer;
        bool isFinished = false;

    public:
        Animation(
            sf::Texture texture,
            int frameWidth,
            int frameHeight,
            int frameCount,
            int times,
            int row
        );

        void Update();
        void Reset();
        bool isAnimationFinished();
        sf::IntRect getSrcRect();

};

#endif