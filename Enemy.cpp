#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>

using namespace std;

// 각 적들의 클래스 
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

        // 벽에 부딪히면 방향 변경
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
    sf::Texture leafTexture; // 양상추 잎 텍스처
    float speed;
    sf::Clock splitClock; // 5초마다 분리되는 타이머
    bool isSplit = false; // 양상추가 분리되었는지 여부

    struct LettuceLeaf {
        sf::Sprite sprite;
        float speed;
    };

    std::vector<LettuceLeaf> lettuceLeaves; // 분리된 양상추 잎들
    //양배추 본체
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
        sprite.setPosition(x, y - 50);  //y값 위로 (양상추잎들과 사진 사이즈가 달라서)
    }

    void move(float leftBound, float rightBound) {
        if (!isSplit) {
            sprite.move(speed, 0);

            // 벽에 부딪히면 방향 변경
            if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
                speed *= -1;
            }
        }
        else {
            // 각 양상추 잎들 독립적으로 이동
            for (auto& leaf : lettuceLeaves) {
                leaf.sprite.move(leaf.speed, 0);

                // 벽에 부딪히면 방향 변경
                if (leaf.sprite.getPosition().x <= leftBound || leaf.sprite.getPosition().x + leaf.sprite.getGlobalBounds().width >= rightBound) {
                    leaf.speed *= -1;
                }
            }
        }
    }

    void attack() {
        if (splitClock.getElapsedTime().asSeconds() >= 7) {
            if (!isSplit) {
                // 7초마다 양상추를 분리
                splitLettuce();
            }
            else {
                // 일정시간이 지나면 다시 원래 상태로 돌아옴
                combineLettuce();
            }
            splitClock.restart();
        }
    }

    void splitLettuce() {  //분리된 양배추
        isSplit = true;
        lettuceLeaves.clear(); // 이전 잎들을 지우고

        // 3개의 양상추 잎으로 분리
        for (int i = 0; i < 3; ++i) {
            LettuceLeaf leaf;
            leaf.sprite.setTexture(leafTexture);  // 양상추 잎 텍스처 사용
            leaf.sprite.setPosition(sprite.getPosition().x + i * 40, sprite.getPosition().y + 50); // 잎 위치 분리
            leaf.speed = (i % 2 == 0) ? speed : -speed; // 각 잎은 반대 방향으로 움직이게 설정
            lettuceLeaves.push_back(leaf);
        }
    }

    void combineLettuce() {
        isSplit = false;
        lettuceLeaves.clear(); // 잎들을 합침
    }

    void draw(sf::RenderWindow& window) {
        if (!isSplit) {
            window.draw(sprite); // 분리되지 않았으면 원래 양상추 그리기
        }
        else {
            for (auto& leaf : lettuceLeaves) {
                window.draw(leaf.sprite); // 분리되었으면 각 잎들 그리기
            }
        }
    }
};



class CheeseEnemy {  // 2층 치즈적 
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float speed;
    sf::Clock cheeseClock; // 치즈 바닥 생성 타이머

    struct CheeseFloor {
        sf::Sprite sprite;
        sf::Clock lifeClock;
    };

    std::vector<CheeseFloor> cheeseFloors; // 치즈 바닥 목록
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

        // 벽에 부딪히면 방향 변경
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed *= -1;
        }
    }

    void spawnCheeseFloor() {
        if (cheeseClock.getElapsedTime().asSeconds() >= 3) { // 3초마다 치즈 바닥 생성
            CheeseFloor floor;
            floor.sprite.setTexture(cheeseFloorTexture);
            floor.sprite.setPosition(sprite.getPosition().x, sprite.getPosition().y + sprite.getGlobalBounds().height - 50);
            cheeseFloors.push_back(floor);
            cheeseClock.restart();
        }
    }

    void updateCheeseFloors() {
        // 치즈 바닥 갱신 (만료된 바닥 제거)
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
                return true; // 충돌 발생
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
    float jumpSpeed = -20.f; // 점프 속도
    float gravity = 2.f; // 중력 (적당한 하강 속도)
    bool isJumping = false;
    bool onGround = true; // 바닥에 있는지 여부
    float groundLevel; // 착지할 층의 Y 좌표
    sf::Clock jumpClock;

    BuneEnemy( float x, float y, float speed)
        : speed(speed), groundLevel(y) {  // y 위치는 착지할 바닥의 Y 좌표
        if (!texture.loadFromFile("img/bune.png")) {
            std::cerr << "Failed to load BuneEnemy texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y);
    }

    void move(float leftBound, float rightBound) {
        sprite.move(speed, 0); // 적은 왼쪽 또는 오른쪽으로 이동

        // 벽에 부딪히면 방향 변경
        if (sprite.getPosition().x <= leftBound || sprite.getPosition().x + sprite.getGlobalBounds().width >= rightBound) {
            speed = -speed;
        }
    }

    void jump() {
        if (isJumping) {
            // 점프 중
            sprite.move(0, jumpSpeed);
            jumpSpeed += gravity; // 중력 적용

            // 착지
            if (sprite.getPosition().y >= groundLevel) {
                sprite.setPosition(sprite.getPosition().x, groundLevel); // 층에 착지
                isJumping = false; // 점프 완료
                jumpSpeed = -20.f; // 점프 속도 초기화
            }
        }
        else if (!isJumping && jumpClock.getElapsedTime().asSeconds() >= 3.f) {
            // 3초마다 점프
            isJumping = true;
            jumpClock.restart();
        }
    }
};