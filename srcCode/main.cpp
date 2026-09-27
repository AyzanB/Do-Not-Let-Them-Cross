#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Animation.h"
#include <string>
#include <cstdlib>
#include <ctime>
#include <vector>

#include <iostream>

float WINDOW_WIDTH = 1280;
float WINDOW_HEIGHT = 720; 

float SPEED = 200;
float FIRE_SPEED = 400;
float BACKGROUND_SPEED = 100;
float GOBLIN_SPEED = 180;

struct EnemyGoblin {
    sf::Sprite sprite;
    bool dead = false;

    EnemyGoblin(sf::Texture& texture) : sprite(texture) {
        sprite.setScale({1.5f, 1.5f});
    }
};

struct PlayerLife {
    sf::Sprite sprite;

    PlayerLife(sf::Texture& texture) : sprite(texture) {}
};

int main() {
    sf::RenderWindow window(sf::VideoMode({(unsigned int) WINDOW_WIDTH, (unsigned int) WINDOW_HEIGHT}), "Don't Let Them Cross");

    sf::Image icon;
    if (!icon.loadFromFile("assets/goblin-icon.png")) {
        return -1;
    }
    
    window.setIcon(icon);
    window.setFramerateLimit(60);

    // SOUNDS

    // background music
    sf::Music bgMusic;
    if (!bgMusic.openFromFile("assets/audio/background.mp3")) {
        return -1;
    }

    bgMusic.setLooping(true);
    bgMusic.setVolume(50);
    bgMusic.play();

    sf::Music gameplayMusic;
    if (!gameplayMusic.openFromFile("assets/audio/gameplay.mp3")) {
        return -1;
    }

    gameplayMusic.setLooping(true);
    gameplayMusic.setVolume(130);
    gameplayMusic.play();

    sf::Music shooterMusic;
    if (!shooterMusic.openFromFile("assets/audio/tank.mp3")) {
        return -1;
    }

    shooterMusic.setLooping(true);
    shooterMusic.setVolume(20);
    shooterMusic.play();

    sf::SoundBuffer bulletFiredBuffer;
    if (!bulletFiredBuffer.loadFromFile("assets/audio/fire.mp3")) {
        return -1;
    }

    sf::Sound bulletFiredSound(bulletFiredBuffer);

    sf::SoundBuffer enemyGoblinShoutBuffer;

    if (!enemyGoblinShoutBuffer.loadFromFile("assets/audio/shout.mp3")) {
        return -1;
    }

    sf::Sound enemyGoblinShoutSound(enemyGoblinShoutBuffer);
    enemyGoblinShoutSound.setVolume(50);

    sf::SoundBuffer enemyGoblinGetPassBuffer;
    if (!enemyGoblinGetPassBuffer.loadFromFile("assets/audio/r-shout.mp3")) {
        return -1;
    }

    sf::Sound enemyGoblinGetPassSound(enemyGoblinGetPassBuffer);
    enemyGoblinGetPassSound.setVolume(50);

    sf::SoundBuffer boomEffectBuffer;
    if (!boomEffectBuffer.loadFromFile("assets/audio/blast.wav")) {
        return -1;
    }

    sf::Sound boomEffectSound(boomEffectBuffer);
    boomEffectSound.setVolume(400);

    sf::SoundBuffer buttonClickBuffer;
    if(!buttonClickBuffer.loadFromFile("assets/audio/blip.wav")) {
        return -1;
    }

    sf::Sound buttonClickSound(buttonClickBuffer);
    buttonClickSound.setVolume(200);

    sf::SoundBuffer enemyHitBuffer;
    if (!enemyHitBuffer.loadFromFile("assets/audio/hit.wav")) {
        return -1;
    }

    sf::Sound enemyHitSound(enemyHitBuffer);

    sf::SoundBuffer shooterDamageBuffer;
    if (!shooterDamageBuffer.loadFromFile("assets/audio/damage.wav")) {
        return -1;
    }

    sf::Sound shooterDamageSound(shooterDamageBuffer);

    // font 
    sf::Font font;
    if (!font.openFromFile("assets/fixedsys.ttf")) {
        return -1;
    }

    // player
    sf::Texture shooterText;
    if (!shooterText.loadFromFile("assets/shooter.png")) {
        return -1;
    }

    Animation animeShooterWalk(shooterText, 32, 32, 3, 6, 0);
    Animation animeShooterFire(shooterText, 32, 32, 3, 6, 1);

    sf::Sprite shooterSprite(shooterText);
    shooterSprite.setScale({2, 2});
    shooterSprite.setPosition({(WINDOW_WIDTH / 2) - (32 / 2), WINDOW_HEIGHT - (WINDOW_HEIGHT / 5)});

    // bullet
    sf::Texture bulletText;
    if (!bulletText.loadFromFile("assets/bullet.png")){
        return -1;
    }

    Animation animeBullet(bulletText, 8, 16, 2, 4, 0);

    sf::Sprite bulletSprite(bulletText);
    bulletSprite.setScale({2, 2});
    bulletSprite.setPosition({(WINDOW_WIDTH / 2) + 8, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5) + 8});

    bool bulletFired = false; 

    // random position
    std::srand(std::time(nullptr));

    int minX = 50;
    int maxX = (int)WINDOW_WIDTH - 50;

    int randomPosX = minX + (std::rand() % (maxX - minX + 1));
    
    // enemy
    sf::Texture enemyGoblinText;
    if (!enemyGoblinText.loadFromFile("assets/enemy.png")) {
        return -1;
    }
    
    Animation animeEnemyGoblin(enemyGoblinText, 32, 32, 4, 8, 0);
    Animation animeEnemyGoblinDead(enemyGoblinText, 32, 32, 1, 0, 1);

    std::vector<EnemyGoblin> enemyGoblins;

    enemyGoblins.emplace_back(enemyGoblinText);
    enemyGoblins.back().sprite.setPosition({(float) randomPosX, -34});

    int currentGoblinSpeed = 0;
    
    // boom effect
    sf::Texture boomEffectText;
    if (!boomEffectText.loadFromFile("assets/boom-effect.png")) {
        return -1;
    }

    Animation animeBoomEffect(boomEffectText, 48, 48, 6, 6, 0);

    sf::Sprite boomEffectSprite(boomEffectText);
    boomEffectSprite.setScale({4.0f, 4.0f});

    bool isBoomEffectActive = false;
    
    int totalKillCount = 0;
    int totalGetPassThroughCount = 0;
    int totalEnemySpawnCount = 0;

    // total kill
    sf::Texture skullIconText;
    if (!skullIconText.loadFromFile("assets/skull-icon.png")) {
        return -1;
    }

    sf::Sprite skullIconSprite(skullIconText);
    skullIconSprite.setPosition({32, 32});

    sf::Text totalKill(font);
    totalKill.setCharacterSize(32);
    totalKill.setFillColor(sf::Color({239, 71, 111}));
    totalKill.setPosition({80, 32 - 8});
    totalKill.setString("X" + std::to_string(totalKillCount));

    // total get pass through
    sf::Texture goblinIconText;
    if (!goblinIconText.loadFromFile("assets/goblin-icon.png")) {
        return -1;
    }

    sf::Sprite goblinIconSprite(goblinIconText);
    goblinIconSprite.setPosition({32, 72});

    sf::Text totalGetPassThrough(font);
    totalGetPassThrough.setCharacterSize(32);
    totalGetPassThrough.setFillColor(sf::Color({6, 214, 160}));
    totalGetPassThrough.setPosition({80, 72 - 8});
    totalGetPassThrough.setString("X" + std::to_string(totalGetPassThroughCount));

    // background
    sf::Texture dungeonBackgroundText1;
    if (!dungeonBackgroundText1.loadFromFile("assets/dungeon.png")) {
        return -1;
    }
    
    sf::Sprite dungeonBackgroundSprite1(dungeonBackgroundText1);
    dungeonBackgroundSprite1.setPosition({0, 0});
    dungeonBackgroundSprite1.setScale({2, 2});

    sf::Sprite dungeonBackgroundSprite2(dungeonBackgroundText1);
    dungeonBackgroundSprite2.setPosition({0, -1440});
    dungeonBackgroundSprite2.setScale({2, 2});

    // state
    bool start = false;
    bool end = false;
    bool pause = false;
    
    // banner
    sf::Texture bannerText;
    if (!bannerText.loadFromFile("assets/shooter-banner.png")) {
        return -1;
    }

    sf::Text messege(font);
    std::string msg = "use 'A', 'B' for move and press 'SPACE' for fire.";
    messege.setString(msg);
    messege.setCharacterSize(24);
    messege.setFillColor(sf::Color::White);
    messege.setPosition({(WINDOW_WIDTH / 2) - (msg.size() * 12 / 2), WINDOW_HEIGHT - 50});

    sf::Sprite bannerSprite(bannerText);
    bannerSprite.setPosition({0, 0});
    bannerSprite.setScale({2, 2});

    // player life
    int damage = 200;

    sf::Text damageText(font);
    damageText.setCharacterSize(24);
    damageText.setPosition({WINDOW_WIDTH - 118, WINDOW_HEIGHT - 272});
    damageText.setFillColor(sf::Color::White);
    damageText.setString(std::to_string(damage) + "/200");

    sf::Texture heartText;
    if (!heartText.loadFromFile("assets/heart.png")) {
        return -1;
    }

    std::vector<PlayerLife> playerLifes;

    for (int i = 0; i < 5; i++) {
        playerLifes.emplace_back(heartText);
    }

    sf::RectangleShape damageBar({32, 200});
    damageBar.setPosition({WINDOW_WIDTH - (32 + 32), WINDOW_HEIGHT - (200 + 32)});
    damageBar.setOutlineThickness(8);
    damageBar.setOutlineColor(sf::Color::Black);
    damageBar.setFillColor(sf::Color::White);

    sf::RectangleShape damageBarFill({32 - 4, 200 - 4});
    damageBarFill.setPosition({(WINDOW_WIDTH + 2) - (32 + 32), (WINDOW_HEIGHT + 2) - (200 + 32)});
    damageBarFill.setFillColor(sf::Color({125, 255, 125}));

    // icons
    sf::Texture playIconText;
    if (!playIconText.loadFromFile("assets/play.png")) {
        return -1;
    }
    
    sf::Sprite playIconSprite(playIconText);
    playIconSprite.setScale({2, 2});
    playIconSprite.setPosition({WINDOW_WIDTH / 2 - 32, WINDOW_HEIGHT / 2 - 32});

    sf::Texture pauseIconText;
    if (!pauseIconText.loadFromFile("assets/pause.png")) {
        return -1;
    }

    sf::Sprite pauseIconSprite(pauseIconText);
    pauseIconSprite.setPosition({WINDOW_WIDTH / 2 - 32, 32});

    sf::Texture replayIconText;
    if (!replayIconText.loadFromFile("assets/replay.png")) {
        return -1;
    }

    sf::Sprite replayIconSprite(replayIconText);
    replayIconSprite.setPosition({WINDOW_WIDTH / 2 - 32, WINDOW_HEIGHT / 2 - 32});

    sf::Clock clock;

    while (window.isOpen()) {
        float deltatime = clock.restart().asSeconds();

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        sf::Vector2i mousePixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(mousePixelPos);
    
        if (!start && !end) {
            shooterMusic.stop();
            gameplayMusic.stop();

            if (playIconSprite.getGlobalBounds().contains({(float) mousePos.x, (float) mousePos.y}) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                start = true;
                
                buttonClickSound.play();
                shooterMusic.play();
                gameplayMusic.play();
                bgMusic.stop();
                enemyGoblinShoutSound.play();
            }
        } else if (start && !end) {
            // player
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
                animeShooterFire.Update();
                shooterSprite.setTextureRect(animeShooterFire.getSrcRect());
            } else if (
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)
            ) {
                animeShooterWalk.Update();
                shooterSprite.setTextureRect(animeShooterWalk.getSrcRect());
            } else {
                animeShooterWalk.Update();
                shooterSprite.setTextureRect(animeShooterWalk.getSrcRect());
            }
    
            animeBullet.Update();
            bulletSprite.setTextureRect(animeBullet.getSrcRect());
    
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                shooterSprite.move({-SPEED * deltatime, 0});
    
                if (!bulletFired) {
                    bulletSprite.move({-SPEED * deltatime, 0});
                }
    
                if (shooterSprite.getPosition().x < -34) {
                    shooterSprite.setPosition({WINDOW_WIDTH, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5)});
                    bulletSprite.setPosition({WINDOW_WIDTH + 24, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5) + 8});
                }
            }
    
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                shooterSprite.move({SPEED * deltatime, 0});
    
                if (!bulletFired) {
                    bulletSprite.move({SPEED * deltatime, 0});
                }
    
                if (shooterSprite.getPosition().x > WINDOW_WIDTH) {
                    shooterSprite.setPosition({-34, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5)});
                    bulletSprite.setPosition({24 - 34, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5) + 8});
                }
            }
    
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space) && !bulletFired) {
                bulletFired = true;

                bulletFiredSound.play();
            }
    
            if ( bulletFired) {
                bulletSprite.move({0, -FIRE_SPEED * deltatime});
            }
    
            if (bulletSprite.getPosition().y < 0) {
                bulletFired = false;
    
                bulletSprite.setPosition({
                    shooterSprite.getPosition().x + 24,
                    shooterSprite.getPosition().y + 8
                });
            }
    
            // enemy
            for (auto& enemyGoblin : enemyGoblins) {
                if (!enemyGoblin.dead) {
                    animeEnemyGoblin.Update();
                    enemyGoblin.sprite.setTextureRect(animeEnemyGoblin.getSrcRect());

                    currentGoblinSpeed = GOBLIN_SPEED;
                } else {
                    animeEnemyGoblinDead.Update();
                    enemyGoblin.sprite.setTextureRect(animeEnemyGoblinDead.getSrcRect());
                    
                    currentGoblinSpeed = BACKGROUND_SPEED;
                }
    
                enemyGoblin.sprite.move({0, currentGoblinSpeed * deltatime});
    
                if (enemyGoblin.sprite.getPosition().y > WINDOW_HEIGHT) {
                    randomPosX = minX + (std::rand() % (maxX - minX + 1));
    
                    enemyGoblin.sprite.setPosition({(float) randomPosX, -34});
                    
                    enemyGoblinShoutSound.play();

                    totalEnemySpawnCount++;
                    enemyGoblin.dead = false;
    
                    totalGetPassThroughCount = totalEnemySpawnCount - totalKillCount;
                    totalGetPassThrough.setString("X" + std::to_string(totalGetPassThroughCount));
                }

                // enemy goblin get pass shooter
                if (enemyGoblin.dead && enemyGoblin.sprite.getPosition().y > shooterSprite.getPosition().y) {
                    enemyGoblinGetPassSound.play();
                }
    
                // dead enemy
                if (!enemyGoblin.dead && bulletSprite.getGlobalBounds().findIntersection(enemyGoblin.sprite.getGlobalBounds())) {
                    enemyGoblin.dead = true;
                    bulletFired = false;

                    enemyHitSound.play();
    
                    bulletSprite.setPosition({
                        shooterSprite.getPosition().x + 24,
                        shooterSprite.getPosition().y + 8
                    });
    
                    totalKillCount++;
                    totalKill.setString("X" + std::to_string(totalKillCount));
    
                    // deficulty speed 
                    if (totalKillCount % 5 == 0) {
                        GOBLIN_SPEED += 10;
                        SPEED += 2;
                        FIRE_SPEED += 4;
                        BACKGROUND_SPEED += 1.5;
                    }

                    if (totalKillCount % 15 == 0) {
                        enemyGoblins.emplace_back(enemyGoblinText);

                        randomPosX = minX + (std::rand() % (maxX - minX + 1));
                        int randomPosY = (-200) + (std::rand() % ((-10) - (-200) + 1));

                        enemyGoblins.back().sprite.setPosition({(float) randomPosX, (float) randomPosY});
                    }

                    // boom effect
                    boomEffectSound.play();
                    isBoomEffectActive = true;
                    animeBoomEffect.Reset();
                    boomEffectSprite.setPosition({enemyGoblin.sprite.getPosition().x - 64, enemyGoblin.sprite.getPosition().y - 64});
                }
                
                // damage
                if (!enemyGoblin.dead && shooterSprite.getGlobalBounds().findIntersection(enemyGoblin.sprite.getGlobalBounds())) {
                    enemyGoblin.dead = true;

                    enemyHitSound.play();
                    shooterDamageSound.play();

                    damage -= 200;

                    damageBarFill.setSize({32 - 4, (float) damage - 4});
                    damageText.setString(std::to_string(damage) + "/200");

                    totalKillCount++;
                    totalKill.setString("X" + std::to_string(totalKillCount));

                    if (damage <= 0) {
                        damage = 50;

                        damageBarFill.setSize({32 - 4, 200 - 4});
                        damageText.setString("200/200");

                        playerLifes.erase(playerLifes.begin());

                        if (playerLifes.size() <= 0) {
                            msg = "Total Kill : x" + std::to_string(totalKillCount) + ", Total Alive : x" + std::to_string(totalGetPassThroughCount);
                            messege.setPosition({(WINDOW_WIDTH / 2) - (msg.size() * 24 / 2), WINDOW_HEIGHT / 2 + 50});
                            messege.setCharacterSize(48);
                            messege.setString(msg);
                            
                            end = true;

                            SPEED = 200;
                            FIRE_SPEED = 400;
                            BACKGROUND_SPEED = 100;
                            GOBLIN_SPEED = 180;

                            totalKillCount = 0;
                            totalGetPassThroughCount = 0;
                            totalEnemySpawnCount = 0;

                            totalKill.setString("X0");
                            totalGetPassThrough.setString("X0");

                            damage = 200;
                            
                            damageBarFill.setSize({32 - 4, 200 - 4});
                            damageText.setString("200/200");

                            playerLifes.clear();

                            for (int i = 0; i < 5; i++) {
                                playerLifes.emplace_back(heartText);
                            }

                            shooterSprite.setPosition({(WINDOW_WIDTH / 2) - (32 / 2), WINDOW_HEIGHT - (WINDOW_HEIGHT / 5)});

                            bulletFired = false;
                            bulletSprite.setPosition({(WINDOW_WIDTH / 2) + 8, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5) + 8});

                            randomPosX = minX + (std::rand() % (maxX - minX + 1));
                            enemyGoblins.clear();
                            enemyGoblins.emplace_back(enemyGoblinText);
                            enemyGoblins.back().sprite.setPosition({(float) randomPosX, -34});
                        }
                    }
                }
            }
            
            if (isBoomEffectActive) {
                animeBoomEffect.Update();
                boomEffectSprite.setTextureRect(animeBoomEffect.getSrcRect());
    
                if (animeBoomEffect.isAnimationFinished()) {
                    isBoomEffectActive = false;
                }
            }
    
            // background
            dungeonBackgroundSprite1.move({0, BACKGROUND_SPEED * deltatime});
            dungeonBackgroundSprite2.move({0, BACKGROUND_SPEED * deltatime});
    
            if (dungeonBackgroundSprite1.getPosition().y >= WINDOW_HEIGHT) {
                dungeonBackgroundSprite1.setPosition({0, dungeonBackgroundSprite2.getPosition().y - 1440});
            }
    
            if (dungeonBackgroundSprite2.getPosition().y >= WINDOW_HEIGHT) {
                dungeonBackgroundSprite2.setPosition({0, dungeonBackgroundSprite1.getPosition().y - 1440});
            }

            // player life 
            for (int i = 0; i < playerLifes.size(); i++) {
                playerLifes[i].sprite.setPosition({WINDOW_WIDTH - (64 * (i+1)), 32});
            }
            
            // end
            replayIconSprite.setScale({1, 1});
            replayIconSprite.setPosition({WINDOW_WIDTH / 2 + 32, 32});

            if (replayIconSprite.getGlobalBounds().contains({(float) mousePos.x, (float) mousePos.y}) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                msg = "Total Kill : x" + std::to_string(totalKillCount) + ", Total Alive : x" + std::to_string(totalGetPassThroughCount);
                messege.setPosition({(WINDOW_WIDTH / 2) - (msg.size() * 24 / 2), WINDOW_HEIGHT / 2 + 50});
                messege.setCharacterSize(48);
                messege.setString(msg);

                buttonClickSound.play();
                
                end = true;

                SPEED = 200;
                FIRE_SPEED = 400;
                BACKGROUND_SPEED = 100;
                GOBLIN_SPEED = 180;

                totalKillCount = 0;
                totalGetPassThroughCount = 0;
                totalEnemySpawnCount = 0;

                totalKill.setString("X0");
                totalGetPassThrough.setString("X0");

                damage = 200;
                
                damageBarFill.setSize({32 - 4, 200 - 4});
                damageText.setString("200/200");

                playerLifes.clear();

                for (int i = 0; i < 5; i++) {
                    playerLifes.emplace_back(heartText);
                }

                shooterSprite.setPosition({(WINDOW_WIDTH / 2) - (32 / 2), WINDOW_HEIGHT - (WINDOW_HEIGHT / 5)});

                bulletFired = false;
                bulletSprite.setPosition({(WINDOW_WIDTH / 2) + 8, WINDOW_HEIGHT - (WINDOW_HEIGHT / 5) + 8});

                randomPosX = minX + (std::rand() % (maxX - minX + 1));
                enemyGoblins.clear();
                enemyGoblins.emplace_back(enemyGoblinText);
                enemyGoblins.back().sprite.setPosition({(float) randomPosX, -34});
            }

            // pause
            if (pauseIconSprite.getGlobalBounds().contains({(float) mousePos.x, (float) mousePos.y}) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                pause = true;
                start = false;
                
                msg = "";
                messege.setString(msg);

                buttonClickSound.play();
                bgMusic.play();
            }
        } else if (start && end) {
            replayIconSprite.setScale({2, 2});
            replayIconSprite.setPosition({WINDOW_WIDTH / 2 - 32, WINDOW_HEIGHT / 2 - 32});

            if (replayIconSprite.getGlobalBounds().contains({(float) mousePos.x, (float) mousePos.y}) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                start = false;
                end = false;

                buttonClickSound.play();
                bgMusic.play();
            }
        }


        window.clear(sf::Color({52, 78, 65}));

        if (!start && !end) {
            window.draw(bannerSprite);
            window.draw(messege);
            window.draw(playIconSprite);
        } else if (start && !end) {
            window.draw(dungeonBackgroundSprite1);
            window.draw(dungeonBackgroundSprite2);
    
            for (auto& enemyGoblin : enemyGoblins) {
                window.draw(enemyGoblin.sprite);
            }
            
            window.draw(bulletSprite);
            window.draw(shooterSprite);
    
            if (isBoomEffectActive) {
                window.draw(boomEffectSprite);
            }
    
            window.draw(skullIconSprite);
            window.draw(totalKill);
            
            for (int i = 0; i < playerLifes.size(); i++) {
                window.draw(playerLifes[i].sprite);
            }
            
            window.draw(damageBar); 
            window.draw(damageBarFill); 
            window.draw(goblinIconSprite);
            window.draw(totalGetPassThrough);
            window.draw(damageText);

            window.draw(pauseIconSprite);
            window.draw(replayIconSprite);
        } else if (start && end) {
            window.draw(bannerSprite);  
            window.draw(messege);   
            window.draw(replayIconSprite);
        }

        window.display();
    }
}