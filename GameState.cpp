// GameState.cpp
#include "GameState.h"

GameState::GameState(sf::RenderWindow& window) : window(window), enemyCnt(10), isBossStage(false) {}

void GameState::update() {
    // �Ϲ� ���� ���¿��� enemyCnt�� 0�� �� ���� ���������� �̵�
    if (enemyCnt <= 0 && !isBossStage) {
        isBossStage = true;
        handleTransition();
    }
}

void GameState::handleTransition() {
    if (isBossStage) {
        // ���� ���������� �̵�
        BossStage bossStage(window);
        bossStage.start();
    }
}
