#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// 화면 크기 정의
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FLOOR_COUNT = 4;  // 층 개수
const float GRAVITY = 0.5f; // 중력
const float JUMP_FORCE = -15.0f; // 점프 힘
const float downForce = 15.0f; // 점프 힘
const float FLOOR_SPACING = 150.0f; // 층 간 간격

// 플레이어 클래스
class Player {
public:
    sf::RectangleShape shape;
    float velocityY = 0.0f; // Y축 속도
    bool isJumping = false; // 점프 상태
    bool isfalling = false; // 점프 상태

    Player(float x, float y, float width, float height) {
        shape.setSize({ width, height });
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
    }

    void jump() {
        if (!isJumping) { // 점프 가능할 때만
            velocityY = JUMP_FORCE; // 위로 점프
            isJumping = true;
        }
    }

    void moveDown() {
        if (!isfalling) { // 점프 가능할 때만
            velocityY = downForce; // 위로 점프
            isfalling = true;
        }
    }

    void applyGravity() {
        velocityY += GRAVITY; // 중력 적용
        shape.move(0, velocityY); // Y축 이동
    }

    void checkCollision(vector<float>& floorPositions) {
        for (float floorY : floorPositions) {
            // 착지 여부 확인
            if (shape.getPosition().y + shape.getSize().y >= floorY &&
                shape.getPosition().y + shape.getSize().y <= floorY + 10 &&
                velocityY >= 0) {
                shape.setPosition(shape.getPosition().x, floorY - shape.getSize().y);
                velocityY = 0;
                isJumping = false; // 착지 처리
                break;
            }
        }
    }


};

// 층 클래스
class Floor {
public:
    sf::RectangleShape shape;

    Floor(float x, float y, float width, float height) {
        shape.setSize({ width, height });
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Green);
    }
};

// 적 클래스
class Enemy {
public:
    sf::RectangleShape shape;

    Enemy(float x, float y, float width, float height) {
        shape.setSize({ width, height });
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Red);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "플레이어 점프 게임");
    window.setFramerateLimit(60);

    // 플레이어 생성: 초기 위치를 1층 위에 설정
    Player player(WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_SPACING - 70, 50, 30);

    // 오프셋 값 추가
    const float FLOOR_OFFSET = 130.0f; // 모든 층을 아래로 내릴 값

    // 층 생성 (1층부터 위로)
    vector<Floor> floors;
    vector<float> floorPositions; // 층 Y 좌표 저장
    for (int i = 0; i < FLOOR_COUNT; i++) {
        // Y 좌표 계산 시 FLOOR_OFFSET 추가
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;
        floors.emplace_back(0, y, WINDOW_WIDTH, 10);
        floorPositions.push_back(y);
    }


    // 적 생성
    vector<Enemy> enemies;
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float x = rand() % (WINDOW_WIDTH - 50);
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING - 50;
        enemies.emplace_back(x, y, 50, 30);
    }

    // 게임 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 플레이어 이동
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.shape.getPosition().x > 0)
            player.shape.move(-5.0f, 0); // 왼쪽 이동
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            player.shape.getPosition().x + player.shape.getSize().x < WINDOW_WIDTH)
            player.shape.move(5.0f, 0); // 오른쪽 이동
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.jump(); // 점프
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.moveDown(); // 아래층 이동

        // 중력 적용
        player.applyGravity();

        // 충돌 및 착지 확인
        player.checkCollision(floorPositions);

        // 화면 갱신
        window.clear();
        for (auto& floor : floors)
            window.draw(floor.shape);
        for (auto& enemy : enemies)
            window.draw(enemy.shape);
        window.draw(player.shape);
        window.display();
    }

    return 0;
}
