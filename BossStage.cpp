#include <SFML/Graphics.hpp>
#include <vector>

class Boss {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture baconShotTexture;
    float speed;
    sf::Clock baconClock; // 베이컨슛 타이머

    struct BaconShot {
        sf::Sprite sprite;
        sf::Vector2f direction;
    };

    std::vector<BaconShot> baconShots;

    Boss(const std::string& bossImg, const std::string& baconImg, float x, float y, float speed) {
        texture.loadFromFile(bossImg);
        baconShotTexture.loadFromFile(baconImg);
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
        this->speed = speed;
    }

    void move(float leftBound, float rightBound) {
        sprite.move(speed, 0);
        if (sprite.getPosition().x <= leftBound ||
            sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1; // 방향 반전
        }
    }

    void baconShot() {
        if (baconClock.getElapsedTime().asSeconds() >= 5.0f) {
            sf::Vector2f pos = sprite.getPosition();
            sf::Vector2f center = { pos.x + sprite.getGlobalBounds().width / 2,
                                   pos.y + sprite.getGlobalBounds().height / 2 };

            // 6방향으로 발사
            std::vector<sf::Vector2f> directions = {
                {0, -1}, {1, 0}, {0, 1}, {-1, 0}, {0.7f, -0.7f}, {-0.7f, -0.7f}
            };
            for (const auto& dir : directions) {
                BaconShot shot;
                shot.sprite.setTexture(baconShotTexture);
                shot.sprite.setPosition(center);
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
};
