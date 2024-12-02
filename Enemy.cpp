#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;

// �� ������ Ŭ���� 
class PattyEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    int hp;

    PattyEnemy(const std::string& textureFile, float x, float y, float speed, int hp)
        : speed(speed), hp(hp) {
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
        if (sprite.getPosition().x <= leftBound ||
            sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }
};

class LettuceEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    sf::Texture leafTexture; // ����� �� �ؽ�ó
    float speed;
    sf::Clock splitClock; // 5�ʸ��� �и��Ǵ� Ÿ�̸�
    bool isSplit = false; // ����߰� �и��Ǿ����� ����

    struct LettuceLeaf {
        sf::Sprite sprite;
        float speed;
    };

    std::vector<LettuceLeaf> lettuceLeaves; // �и��� ����� �ٵ�
    //����� ��ü
    LettuceEnemy(float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile("img/lettuce.png")) {
            std::cerr << "Failed to load LettuceEnemy texture!" << std::endl;
            exit(-1);
        }
        if (!leafTexture.loadFromFile("img/lettuces.png")) {
            std::cerr << "Failed to load LettuceLeaf texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y - 50);  //y�� ���� (������ٵ�� ���� ����� �޶�)
    }

    void move(float leftBound, float rightBound) {
        if (!isSplit) {
            sprite.move(speed, 0);

            // ���� �ε����� ���� ����
            if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
                speed *= -1;
            }
        }
        else {
            // �� ����� �ٵ� ���������� �̵�
            for (auto& leaf : lettuceLeaves) {
                leaf.sprite.move(leaf.speed, 0);

                // ���� �ε����� ���� ����
                if (leaf.sprite.getPosition().x <= leftBound || leaf.sprite.getPosition().x + leaf.sprite.getGlobalBounds().width >= rightBound) {
                    leaf.speed *= -1;
                }
            }
        }
    }

    void attack() {
        if (splitClock.getElapsedTime().asSeconds() >= 7) {
            if (!isSplit) {
                // 7�ʸ��� ����߸� �и�
                splitLettuce();
            }
            else {
                // �����ð��� ������ �ٽ� ���� ���·� ���ƿ�
                combineLettuce();
            }
            splitClock.restart();
        }
    }

    void splitLettuce() {  //�и��� �����
        isSplit = true;
        lettuceLeaves.clear(); // ���� �ٵ��� �����

        // 3���� ����� ������ �и�
        for (int i = 0; i < 3; ++i) {
            LettuceLeaf leaf;
            leaf.sprite.setTexture(leafTexture);  // ����� �� �ؽ�ó ���
            leaf.sprite.setPosition(sprite.getPosition().x + i * 40, sprite.getPosition().y + 50); // �� ��ġ �и�
            leaf.speed = (i % 2 == 0) ? speed : -speed; // �� ���� �ݴ� �������� �����̰� ����
            lettuceLeaves.push_back(leaf);
        }
    }

    void combineLettuce() {
        isSplit = false;
        lettuceLeaves.clear(); // �ٵ��� ��ħ
    }

    void draw(sf::RenderWindow& window) {
        if (!isSplit) {
            window.draw(sprite); // �и����� �ʾ����� ���� ����� �׸���
        }
        else {
            for (auto& leaf : lettuceLeaves) {
                window.draw(leaf.sprite); // �и��Ǿ����� �� �ٵ� �׸���
            }
        }
    }
};



class CheeseEnemy {  // 2�� ġ���� 
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    sf::Clock cheeseClock; // ġ�� �ٴ� ���� Ÿ�̸�

    struct CheeseFloor {
        sf::Sprite sprite;
        sf::Clock lifeClock;
    };

    std::vector<CheeseFloor> cheeseFloors; // ġ�� �ٴ� ���
    sf::Texture cheeseFloorTexture;

    CheeseEnemy(float x, float y, float speed)
        : speed(speed) {
        if (!texture.loadFromFile("img/cheese.png")) {
            std::cerr << "Failed to load CheeseEnemy texture!" << std::endl;
            exit(-1);
        }
        if (!cheeseFloorTexture.loadFromFile("img/cheesefloor.png")) {
            std::cerr << "Failed to load CheeseFloor texture!" << std::endl;
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

    void spawnCheeseFloor() {
        if (cheeseClock.getElapsedTime().asSeconds() >= 3) { // 3�ʸ��� ġ�� �ٴ� ����
            CheeseFloor floor;
            floor.sprite.setTexture(cheeseFloorTexture);
            floor.sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + sprite.getGlobalBounds().height - 50);
            cheeseFloors.push_back(floor);
            cheeseClock.restart();
        }
    }

    void updateCheeseFloors() {
        // ġ�� �ٴ� ���� (����� �ٴ� ����)
        cheeseFloors.erase(std::remove_if(cheeseFloors.begin(), cheeseFloors.end(),
            [](const CheeseFloor& floor) { return floor.lifeClock.getElapsedTime().asSeconds() > 5; }),
            cheeseFloors.end());
    }

    void drawCheeseFloors(sf::RenderWindow& window) {
        for (auto& floor : cheeseFloors) {
            window.draw(floor.sprite);
        }
    }

    bool checkPlayerCollision(const sf::Sprite& player) {
        for (auto& floor : cheeseFloors) {
            if (player.getGlobalBounds().intersects(floor.sprite.getGlobalBounds())) {
                return true; // �浹 �߻�
            }
        }
        return false;
    }
};


class BuneEnemy {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    float jumpSpeed = -20.f; // ���� �ӵ�
    float gravity = 2.f; // �߷� (������ �ϰ� �ӵ�)
    bool isJumping = false;
    bool onGround = true; // �ٴڿ� �ִ��� ����
    float groundLevel; // ������ ���� Y ��ǥ
    sf::Clock jumpClock;

    BuneEnemy( float x, float y, float speed)
        : speed(speed), groundLevel(y) {  // y ��ġ�� ������ �ٴ��� Y ��ǥ
        if (!texture.loadFromFile("img/bune.png")) {
            std::cerr << "Failed to load BuneEnemy texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move(float leftBound, float rightBound) {
        sprite.move(speed, 0); // ���� ���� �Ǵ� ���������� �̵�

        // ���� �ε����� ���� ����
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed = -speed;
        }
    }

    void jump() {
        if (isJumping) {
            // ���� ��
            sprite.move(0, jumpSpeed);
            jumpSpeed += gravity; // �߷� ����

            // ����
            if (sprite.getPosition().y >= groundLevel) {
                sprite.setPosition(sprite.getPosition().x, groundLevel); // ���� ����
                isJumping = false; // ���� �Ϸ�
                jumpSpeed = -20.f; // ���� �ӵ� �ʱ�ȭ
            }
        }
        else if (!isJumping && jumpClock.getElapsedTime().asSeconds() >= 3.f) {
            // 3�ʸ��� ����
            isJumping = true;
            jumpClock.restart();
        }
    }
};