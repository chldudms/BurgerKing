// GameState.cpp
#include "GameState.h"

GameState::GameState(sf::RenderWindow& window) : window(window), enemyCnt(10), isBossStage(false) {}

void GameState::update() {
    // 일반 게임 상태에서 enemyCnt가 0일 때 보스 스테이지로 이동
    if (enemyCnt <= 0 && !isBossStage) {
        isBossStage = true;
        handleTransition();
    }
}

void GameState::handleTransition() {
    if (isBossStage) {
        // 보스 스테이지로 이동
        BossStage bossStage(window);
        bossStage.start();
    }
}
