#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// ȭ�� ũ�� ����
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

// �� ���� ���� �� Ŭ������

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

        // ���� �ε����� ���� ����
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void attack() {
        // 2���� ġ�� ���� ������ �Դ� ���� �ϸ� ����
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

        // ���� �ε����� ���� ����
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void attack() {
        // 4���� ��Ƽ ���� ������ ��ġ���� ����
        sprite.move(0, (rand() % 2 == 0) ? -5.0f : 5.0f);  // �������� ����
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

        // ���� �ε����� ���� ����
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void attack() {
        // 3���� ���� ���� �밢������ �̵�
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

        // ���� �ε����� ���� ����
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void attack() {
        // 1���� �γ� ���� ������ �̵�
        sprite.move(speed, 0);
    }
};

// �� Ŭ����
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "�÷��̾� ���� ����");
    window.setFramerateLimit(60);

    // ��� �ؽ�ó �� ��������Ʈ
    sf::Texture backgroundTexture;
    if (!backgroundTexture.loadFromFile("img/background.png")) {
        std::cerr << "Failed to load background texture!" << std::endl;
        return -1;
    }
    sf::Sprite backgroundSprite(backgroundTexture);

    // �÷��̾� ����
    Player player("img/burger0.png", WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_SPACING - 70);

    // �� ����
    vector<Floor> floors;
    vector<float> floorPositions;
    const float FLOOR_OFFSET = 150.0f;
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET - 50;  // ���� ���� �� ���� �ø�
        floors.emplace_back(0, y, WINDOW_WIDTH, 30);
        floorPositions.push_back(y);
    }

    // �� ���� ������ �� ����
    vector<BuneEnemy> buneEnemies;
    vector<CheeseEnemy> cheeseEnemies;
    vector<LettuceEnemy> lettuceEnemies;
    vector<PattyEnemy> pattyEnemies;

    // 1�� �γ� �� ����
    buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 50, 2.0f);
    // 2�� ġ�� �� ����
    cheeseEnemies.emplace_back("img/cheese.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 50, 3.0f);
    // 3�� ���� �� ����
    lettuceEnemies.emplace_back("img/lettuce.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 50, 4.0f);
    // 4�� ��Ƽ �� ����
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 50, 5.0f);

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

        // �߷� ����
        player.applyGravity();

        // �浹 Ȯ��
        player.checkCollision(floorPositions);

        // �� �� �̵� �� ����
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

        // ȭ�� ����
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
