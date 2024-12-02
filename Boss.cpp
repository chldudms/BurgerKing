#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;

class Boss {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture baconShotTexture;
    float speed;
    sf::Clock baconClock; // �������� Ÿ�̸�
    int hp; // ���� ü��

    struct BaconShot {
        sf::Sprite sprite;
        sf::Vector2f direction;
    };

    std::vector<BaconShot> baconShots;

    // ŷ���� ������: ü�� 10
    Boss(const std::string& bossImg, const std::string& baconImg, float x, float y, float speed)
        : hp(10), speed(speed) {
        texture.loadFromFile(bossImg);
        baconShotTexture.loadFromFile(baconImg);
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    // ���񺸽� ������: ü�� 100
    Boss(const std::string& bossImg, float x, float y, float speed)
        : hp(100), speed(speed) {
        texture.loadFromFile(bossImg);
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move(float leftBound, float rightBound) {
        sprite.move(speed, 0);
        if (sprite.getPosition().x <= leftBound ||
            sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1; // ���� ����
        }
    }

    void baconShot() {
        if (baconClock.getElapsedTime().asSeconds() >= 3.0f) {
            sf::Vector2f pos = sprite.getPosition();
            sf::Vector2f center = { pos.x + sprite.getGlobalBounds().width / 2,
                                    pos.y + sprite.getGlobalBounds().height / 2 };

            // 6�������� �߻�
            std::vector<sf::Vector2f> directions = {
                {0, -100},   // ��
                {1, 0},    // ������
                {0, 1},    // �Ʒ�
                {-1, 0},   // ����
                {0.7f, -0.7f},  // ������ ��
                {-0.7f, -0.7f}  // ���� ��
            };

            for (const auto& dir : directions) {
                BaconShot shot;
                shot.sprite.setTexture(baconShotTexture);
                shot.sprite.setPosition(center); // �߻� ��ġ�� �߾����� ����
                shot.direction = dir;
                baconShots.push_back(shot);
            }

            baconClock.restart();
        }

        // ������ �̵�
        for (auto it = baconShots.begin(); it != baconShots.end();) {
            it->sprite.move(it->direction * 5.0f); // �ӵ� 5.0f
            if (it->sprite.getPosition().x < 0 || it->sprite.getPosition().x > 800 ||
                it->sprite.getPosition().y < 0 || it->sprite.getPosition().y > 600) {
                it = baconShots.erase(it); // ȭ�� ������ ������ ����
            }
            else {
                ++it;
            }
        }
    }

    void draw(sf::RenderWindow& window) {
        window.draw(sprite);
        for (auto& shot : baconShots) {
            window.draw(shot.sprite);
        }
    }

    // ������ ���ظ� ���� ��
    void takeDamage() {
        if (hp > 0) {
            hp--; // ü���� 1 ����
        }
    }

    bool isDead() const {
        return hp <= 0;  // ������ �׾����� true ��ȯ
    }

};
