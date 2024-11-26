#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// 화면 크기 정의
const int WINDOW_WIDTH = 1300;
const int WINDOW_HEIGHT = 800;
const int FLOOR_COUNT = 4;
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -15.0f;
const float DOWN_FORCE = 10.0f;
const float FLOOR_SPACING = 200.0f;  // 층 간 간격을 줄여서 화면에 다 들어오게 조정
const float FLOOR_THICKNESS = 40.0f;  // 층 두께 설정

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    bool isDown = false;
    int hp = 3; // 플레이어 체력

    Player(const std::string& textureFile, float x, float y) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load player texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void jump() {
        if (!isJumping) {
            velocityY = JUMP_FORCE;
            isJumping = true;
        }
    }

    void moveDown() {
        if (!isDown) {
            velocityY = DOWN_FORCE;
            isDown = true;
        }
    }

    void applyGravity() {
        velocityY += GRAVITY;
        sprite.move(0, velocityY);
    }

    void checkCollision(const std::vector<float>& floorPositions) {
        for (float floorY : floorPositions) {
            if (sprite.getPosition().y + sprite.getGlobalBounds().height >= floorY &&
                sprite.getPosition().y + sprite.getGlobalBounds().height <= floorY + 10 &&
                velocityY >= 0) {
                sprite.setPosition(sprite.getPosition().x, floorY - sprite.getGlobalBounds().height);
                velocityY = 0;
                isJumping = false;
                isDown = false;
                break;
            }
        }
    }

    void takeDamage() {
        hp--;
        if (hp <= 0) {
            std::cout << "Game Over!" << std::endl;
            exit(0);  // 게임 오버 처리
        }
    }
};

class Missile {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

    Missile(const std::string& textureFile, float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load missile texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move() {
        sprite.move(0, -speed); // 미사일은 위로 이동
    }

    bool isOffScreen() {
        return sprite.getPosition().y < 0; // 화면 밖으로 나갔을 경우
    }

    bool isHitByMissile(const sf::Sprite& enemySprite) {
        return sprite.getGlobalBounds().intersects(enemySprite.getGlobalBounds());
    }
};

// 각 층에 따른 적 클래스들

class CheeseEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    bool isAlive = true;

    CheeseEnemy(const std::string& textureFile, float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load CheeseEnemy texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move(float leftBound, float rightBound) {
        sprite.move(speed, 0);

        // 벽에 부딪히면 방향 변경
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void takeDamage() {
        isAlive = false;  // 적이 맞으면 죽음
    }
};

class Floor {
public:
    sf::RectangleShape shape;

    Floor(float x, float y, float width, float height) {
        shape.setSize({ width, height });
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::White);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "플레이어 점프 게임");
    window.setFramerateLimit(60);

    // 배경 텍스처 및 스프라이트
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("img/back.jpg")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // 플레이어 생성
    Player player("img/burger0.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_SPACING - 70);

    // 층 생성
    vector<Floor> floors;
    vector<float> floorPositions;
    const float FLOOR_OFFSET = 150.0f;  // 층 오프셋 설정
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;  // 층 간 간격을 줄여서 적절히 배치
        floors.emplace_back(0, y, WINDOW_WIDTH, FLOOR_THICKNESS);  // 층 두께 설정
        floorPositions.push_back(y);
    }

    // 각 층에 고유한 적 생성
    vector<CheeseEnemy> cheeseEnemies;

    // 2층 치즈 적 생성
    cheeseEnemies.emplace_back("img/cheese.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (FLOOR_SPACING * 2) + FLOOR_OFFSET - 110, 3.0f);

    // 미사일 저장용 벡터
    vector<Missile> missiles;

    // 게임 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 플레이어 이동
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.sprite.getPosition().x > 0)
            player.sprite.move(-5.0f, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            player.sprite.getPosition().x + player.sprite.getGlobalBounds().width < WINDOW_WIDTH)
            player.sprite.move(5.0f, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.jump();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.moveDown();

        // 미사일 발사
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            missiles.emplace_back("img/fire.png", player.sprite.getPosition().x + player.sprite.getGlobalBounds().width / 2 - 5, player.sprite.getPosition().y, 5.0f);
        }

        // 중력 적용
        player.applyGravity();

        // 충돌 확인
        player.checkCollision(floorPositions);

        // 각 적 이동
        for (auto& enemy : cheeseEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }

        // 미사일 이동 및 적과 충돌 확인
        for (auto& missile : missiles) {
            missile.move();
            for (auto& enemy : cheeseEnemies) {
                if (missile.isHitByMissile(enemy.sprite)) {
                    enemy.takeDamage();
                    missile.sprite.setPosition(-100, -100); // 미사일을 화면 밖으로 이동
                }
            }
        }

        // 화면 갱신
        window.clear();
        window.draw(backgroundSprite);
        for (auto& floor : floors)
            window.draw(floor.shape);
        for (auto& enemy : cheeseEnemies)
            window.draw(enemy.sprite);
        for (auto& missile : missiles)
            window.draw(missile.sprite);
        window.draw(player.sprite);
        window.display();
    }

    return 0;
}
