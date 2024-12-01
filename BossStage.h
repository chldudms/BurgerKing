// BossStage.h
#pragma once
#include <SFML/Graphics.hpp>

class BossStage {
public:
    BossStage(sf::RenderWindow& window);
    void start();
    void update();

private:
    sf::RenderWindow& window;
    sf::Sprite bossSprite;
    bool isBossDefeated;
};

