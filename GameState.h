// GameState.h
#pragma once

#include "BossStage.h"

class GameState {
public:
    GameState(sf::RenderWindow& window);
    void update();
    void handleTransition();

private:
    sf::RenderWindow& window;
    int enemyCnt;
    bool isBossStage;
};

