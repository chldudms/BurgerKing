#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// ȭ�� ũ�� ����
const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int FLOOR_COUNT = 4;  // �� ����
const float GRAVITY = 0.5f; // �߷�
const float JUMP_FORCE = -15.0f; // ���� ��
const float downForce = 15.0f; // ���� ��
const float FLOOR_SPACING = 150.0f; // �� �� ����

// �÷��̾� Ŭ����
class Player {
public:
    sf::RectangleShape shape;
    float velocityY = 0.0f; // Y�� �ӵ�
    bool isJumping = false; // ���� ����
    bool isfalling = false; // ���� ����

    Player(float x, float y, float width, float height) {
        shape.setSize({ width, height });
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Blue);
    }

    void jump() {
        if (!isJumping) { // ���� ������ ����
            velocityY = JUMP_FORCE; // ���� ����
            isJumping = true;
        }
    }

    void moveDown() {
        if (!isfalling) { // ���� ������ ����
            velocityY = downForce; // ���� ����
            isfalling = true;
        }
    }

    void applyGravity() {
        velocityY += GRAVITY; // �߷� ����
        shape.move(0, velocityY); // Y�� �̵�
    }

    void checkCollision(vector<float>& floorPositions) {
        for (float floorY : floorPositions) {
            // ���� ���� Ȯ��
            if (shape.getPosition().y + shape.getSize().y >= floorY &&
                shape.getPosition().y + shape.getSize().y <= floorY + 10 &&
                velocityY >= 0) {
                shape.setPosition(shape.getPosition().x, floorY - shape.getSize().y);
                velocityY = 0;
                isJumping = false; // ���� ó��
                break;
            }
        }
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

// �� Ŭ����
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "�÷��̾� ���� ����");
    window.setFramerateLimit(60);

    // �÷��̾� ����: �ʱ� ��ġ�� 1�� ���� ����
    Player player(WINDOW_WIDTH / 2, WINDOW_HEIGHT - FLOOR_SPACING - 70, 50, 30);

    // ������ �� �߰�
    const float FLOOR_OFFSET = 130.0f; // ��� ���� �Ʒ��� ���� ��

    // �� ���� (1������ ����)
    vector<Floor> floors;
    vector<float> floorPositions; // �� Y ��ǥ ����
    for (int i = 0; i < FLOOR_COUNT; i++) {
        // Y ��ǥ ��� �� FLOOR_OFFSET �߰�
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;
        floors.emplace_back(0, y, WINDOW_WIDTH, 10);
        floorPositions.push_back(y);
    }


    // �� ����
    vector<Enemy> enemies;
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float x = rand() % (WINDOW_WIDTH - 50);
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING - 50;
        enemies.emplace_back(x, y, 50, 30);
    }

    // ���� ����
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // �÷��̾� �̵�
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.shape.getPosition().x > 0)
            player.shape.move(-5.0f, 0); // ���� �̵�
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            player.shape.getPosition().x + player.shape.getSize().x < WINDOW_WIDTH)
            player.shape.move(5.0f, 0); // ������ �̵�
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.jump(); // ����
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.moveDown(); // �Ʒ��� �̵�

        // �߷� ����
        player.applyGravity();

        // �浹 �� ���� Ȯ��
        player.checkCollision(floorPositions);

        // ȭ�� ����
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
