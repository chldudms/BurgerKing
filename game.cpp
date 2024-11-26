#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// ȭ�� ũ�� ����
const int WINDOW_WIDTH = 1300;
const int WINDOW_HEIGHT = 800;
const int FLOOR_COUNT = 4;
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -15.0f;
const float DOWN_FORCE = 10.0f;
const float FLOOR_SPACING = 200.0f;  // �� �� ������ �ٿ��� ȭ�鿡 �� ������ ����
const float FLOOR_THICKNESS = 40.0f;  // �� �β� ����

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    bool isDown = false;
    int hp = 3; // �÷��̾� ü��

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
            exit(0);  // ���� ���� ó��
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
        sprite.move(0, -speed); // �̻����� ���� �̵�
    }

    bool isOffScreen() {
        return sprite.getPosition().y < 0; // ȭ�� ������ ������ ���
    }

    bool isHitByMissile(const sf::Sprite& enemySprite) {
        return sprite.getGlobalBounds().intersects(enemySprite.getGlobalBounds());
    }
};

// �� ���� ���� �� Ŭ������

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

        // ���� �ε����� ���� ����
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void takeDamage() {
        isAlive = false;  // ���� ������ ����
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "�÷��̾� ���� ����");
    window.setFramerateLimit(60);

    // ��� �ؽ�ó �� ��������Ʈ
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("img/back.jpg")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // �÷��̾� ����
    Player player("img/burger0.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_SPACING - 70);

    // �� ����
    vector<Floor> floors;
    vector<float> floorPositions;
    const float FLOOR_OFFSET = 150.0f;  // �� ������ ����
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;  // �� �� ������ �ٿ��� ������ ��ġ
        floors.emplace_back(0, y, WINDOW_WIDTH, FLOOR_THICKNESS);  // �� �β� ����
        floorPositions.push_back(y);
    }

    // �� ���� ������ �� ����
    vector<CheeseEnemy> cheeseEnemies;

    // 2�� ġ�� �� ����
    cheeseEnemies.emplace_back("img/cheese.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (FLOOR_SPACING * 2) + FLOOR_OFFSET - 110, 3.0f);

    // �̻��� ����� ����
    vector<Missile> missiles;

    // ���� ����
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // �÷��̾� �̵�
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.sprite.getPosition().x > 0)
            player.sprite.move(-5.0f, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            player.sprite.getPosition().x + player.sprite.getGlobalBounds().width < WINDOW_WIDTH)
            player.sprite.move(5.0f, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.jump();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.moveDown();

        // �̻��� �߻�
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            missiles.emplace_back("img/fire.png", player.sprite.getPosition().x + player.sprite.getGlobalBounds().width / 2 - 5, player.sprite.getPosition().y, 5.0f);
        }

        // �߷� ����
        player.applyGravity();

        // �浹 Ȯ��
        player.checkCollision(floorPositions);

        // �� �� �̵�
        for (auto& enemy : cheeseEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }

        // �̻��� �̵� �� ���� �浹 Ȯ��
        for (auto& missile : missiles) {
            missile.move();
            for (auto& enemy : cheeseEnemies) {
                if (missile.isHitByMissile(enemy.sprite)) {
                    enemy.takeDamage();
                    missile.sprite.setPosition(-100, -100); // �̻����� ȭ�� ������ �̵�
                }
            }
        }

        // ȭ�� ����
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
