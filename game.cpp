#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// 화면 크기 정의
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 800;
const int FLOOR_COUNT = 4;
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -15.0f;
const float downForce = 15.0f;
const float FLOOR_SPACING = 150.0f;

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    bool isDown = false;

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
            velocityY = downForce;
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
};

// 각 층에 따른 적 클래스들

class CheeseEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

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

    void attack() {
        // 2층의 치즈 적은 빠르게 왔다 갔다 하며 공격
        sprite.move(speed, 0);
    }
};

class PattyEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

    PattyEnemy(const std::string& textureFile, float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load PattyEnemy texture!" << std::endl;
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

    void attack() {
        // 4층의 패티 적은 일정한 위치에서 점프
        sprite.move(0, (rand() % 2 == 0) ? -5.0f : 5.0f);  // 간헐적인 점프
    }
};

class LettuceEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

    LettuceEnemy(const std::string& textureFile, float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load LettuceEnemy texture!" << std::endl;
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

    void attack() {
        // 3층의 상추 적은 대각선으로 이동
        sprite.move(speed, 0);
    }
};

class BuneEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;

    BuneEnemy(const std::string& textureFile, float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load BuneEnemy texture!" << std::endl;
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

    void attack() {
        // 1층의 부네 적은 느리게 이동
        sprite.move(speed, 0);
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

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "플레이어 점프 게임");
    window.setFramerateLimit(60);

    // 배경 텍스처 및 스프라이트
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("img/background.png")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // 플레이어 생성
    Player player("img/burger0.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_SPACING - 70);

    // 층 생성
    vector<Floor> floors;
    vector<float> floorPositions;
    const float FLOOR_OFFSET = 150.0f;
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET - 50;  // 층을 조금 더 위로 올림
        floors.emplace_back(0, y, WINDOW_WIDTH, 30);
        floorPositions.push_back(y);
    }

    // 각 층에 고유한 적 생성
    vector<BuneEnemy> buneEnemies;
    vector<CheeseEnemy> cheeseEnemies;
    vector<LettuceEnemy> lettuceEnemies;
    vector<PattyEnemy> pattyEnemies;

    // 1층 부네 적 생성
    buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 50, 2.0f);
    // 2층 치즈 적 생성
    cheeseEnemies.emplace_back("img/cheese.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 50, 3.0f);
    // 3층 상추 적 생성
    lettuceEnemies.emplace_back("img/lettuce.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 50, 4.0f);
    // 4층 패티 적 생성
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 50, 5.0f);

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

        // 중력 적용
        player.applyGravity();

        // 충돌 확인
        player.checkCollision(floorPositions);

        // 각 적 이동 및 공격
        for (auto& enemy : buneEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            enemy.attack();
        }
        for (auto& enemy : cheeseEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            enemy.attack();
        }
        for (auto& enemy : lettuceEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            enemy.attack();
        }
        for (auto& enemy : pattyEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            enemy.attack();
        }

        // 화면 갱신
        window.clear();
        window.draw(backgroundSprite);
        for (auto& floor : floors)
            window.draw(floor.shape);
        for (auto& enemy : buneEnemies)
            window.draw(enemy.sprite);
        for (auto& enemy : cheeseEnemies)
            window.draw(enemy.sprite);
        for (auto& enemy : lettuceEnemies)
            window.draw(enemy.sprite);
        for (auto& enemy : pattyEnemies)
            window.draw(enemy.sprite);
        window.draw(player.sprite);
        window.display();
    }

    return 0;
}
