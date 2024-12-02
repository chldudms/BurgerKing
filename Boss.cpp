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
    sf::Clock baconClock; // 베이컨슛 타이머
    int hp; // 보스 체력

    struct BaconShot {
        sf::Sprite sprite;
        sf::Vector2f direction;
    };

    std::vector<BaconShot> baconShots;

    // 킹보스 생성자: 체력 10
    Boss(const std::string& bossImg, const std::string& baconImg, float x, float y, float speed)
        : hp(10), speed(speed) {
        texture.loadFromFile(bossImg);
        baconShotTexture.loadFromFile(baconImg);
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    // 좀비보스 생성자: 체력 100
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
            speed *= -1; // 방향 반전
        }
    }

    void baconShot() {
        if (baconClock.getElapsedTime().asSeconds() >= 3.0f) {
            sf::Vector2f pos = sprite.getPosition();
            sf::Vector2f center = { pos.x + sprite.getGlobalBounds().width / 2,
                                    pos.y + sprite.getGlobalBounds().height / 2 };

            // 6방향으로 발사
            std::vector<sf::Vector2f> directions = {
                {0, -100},   // 위
                {1, 0},    // 오른쪽
                {0, 1},    // 아래
                {-1, 0},   // 왼쪽
                {0.7f, -0.7f},  // 오른쪽 위
                {-0.7f, -0.7f}  // 왼쪽 위
            };

            for (const auto& dir : directions) {
                BaconShot shot;
                shot.sprite.setTexture(baconShotTexture);
                shot.sprite.setPosition(center); // 발사 위치를 중앙으로 설정
                shot.direction = dir;
                baconShots.push_back(shot);
            }

            baconClock.restart();
        }

        // 베이컨 이동
        for (auto it = baconShots.begin(); it != baconShots.end();) {
            it->sprite.move(it->direction * 5.0f); // 속도 5.0f
            if (it->sprite.getPosition().x < 0 || it->sprite.getPosition().x > 800 ||
                it->sprite.getPosition().y < 0 || it->sprite.getPosition().y > 600) {
                it = baconShots.erase(it); // 화면 밖으로 나가면 제거
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

    // 보스가 피해를 입을 때
    void takeDamage() {
        if (hp > 0) {
            hp--; // 체력을 1 감소
        }
    }

    bool isDead() const {
        return hp <= 0;  // 보스가 죽었으면 true 반환
    }

};
