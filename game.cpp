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
int lastDirection = 1;  // 1: 오른쪽, -1: 왼쪽 (기본값은 오른쪽)   
 int enemyCnt = 50;

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    bool isDown = false;
    int hp = 3;             // 플레이어 체력
    float moveSpeed = 5.0f; // 플레이어 이동 속도

    Player(const std::string& textureFile, float x, float y, float moveSpeed = 5.0f)
        : moveSpeed(moveSpeed) {
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

    void moveLeft() {
        sprite.move(-moveSpeed, 0);
        lastDirection = -1; // 왼쪽으로 이동
    }

    void moveRight() {
        sprite.move(moveSpeed, 0);
        lastDirection = 1; // 오른쪽으로 이동
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
       
        sprite.move(speed, 0);
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
  
        sprite.move(speed, 0);
    }
};

class Missile {
public:
    sf::CircleShape shape;
    float speed;
    int direction; // 미사일의 방향 (-1: 왼쪽, 1: 오른쪽)

    // 생성자: 미사일 방향 초기화
    Missile(float x, float y, float speed, int direction)
        : speed(speed), direction(direction) {
        shape.setRadius(10.0f); // 미사일 크기
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y + 60.0f); // y값을 조정하여 발사 위치 설정
    }

    // 미사일 이동
    void move() {
        shape.move(speed * direction, 0); // 미사일 방향에 따라 이동
    }

    // 화면 밖으로 나갔는지 확인
    bool isOffScreen() {
        return shape.getPosition().x < 0 || shape.getPosition().x > WINDOW_WIDTH;
    }

    // 적과 충돌했는지 확인
    bool isHitByMissile(const sf::Sprite& enemySprite) {
        return shape.getGlobalBounds().intersects(enemySprite.getGlobalBounds());
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT),"BurgerKing");
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
    vector<BuneEnemy> buneEnemies;
    vector<CheeseEnemy> cheeseEnemies;
    vector<LettuceEnemy> lettuceEnemies;
    vector<PattyEnemy> pattyEnemies;

    // 1층 번 적 생성
    buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 70, 2.0f);
    // 2층 치즈 적 생성
    cheeseEnemies.emplace_back("img/cheese.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);
    // 3층 상추 적 생성
    lettuceEnemies.emplace_back("img/lettuce.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);
    // 4층 패티 적 생성
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 5.0f);


    // 미사일 
    vector<Missile> missiles;

    // 미사일 발사 제한 시간
    sf::Clock missileClock;
    const float MISSILE_COOLDOWN = 1.0f; //1초 
    
    //남은 적 텍스트로 띄우기
        sf::Font font;
        if (!font.loadFromFile("fonts/MaplestoryBold.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            return -1;
        }
        //남은 적 텍스트 설정
        sf::Text enemyCountText;
        enemyCountText.setFont(font);
        enemyCountText.setCharacterSize(50);
        enemyCountText.setFillColor(sf::Color::Red);
        enemyCountText.setPosition(10, 60);

    // 게임 루프
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // 플레이어 이동
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.sprite.getPosition().x > 0)
            player.moveLeft(); // ← 함수 호출로 수정
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            player.sprite.getPosition().x + player.sprite.getGlobalBounds().width < WINDOW_WIDTH)
            player.moveRight(); // → 함수 호출로 수정
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.jump();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.moveDown();


   // 중력 적용  // 충돌 확인
        player.applyGravity();
        player.checkCollision(floorPositions);

        // 적과 플레이어 충돌 처리
        for (auto& enemy : buneEnemies) {
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                player.takeDamage();
            }
        }
        for (auto& enemy : cheeseEnemies) { 
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                player.takeDamage();
            }
        }
        for (auto& enemy : lettuceEnemies) {
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                player.takeDamage();
            }
        }
        for (auto& enemy : pattyEnemies) {
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                player.takeDamage();
            }
        }


        //스페이스 눌렀을때 미사일 발사 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (missileClock.getElapsedTime().asSeconds() > MISSILE_COOLDOWN) {
                // 미사일 생성 시 플레이어의 위치와 방향을 저장
                missiles.emplace_back(
                    player.sprite.getPosition().x + player.sprite.getGlobalBounds().width / 2,
                    player.sprite.getPosition().y,
                    8.0f, // 미사일 속도
                    lastDirection // 플레이어의 마지막 방향 저장
                );
                missileClock.restart();
            }
        }


        // 미사일 이동
        for (auto it = missiles.begin(); it != missiles.end();) {
            it->move(); // 각 미사일은 자신의 방향으로 이동
            if (it->isOffScreen()) {
                it = missiles.erase(it); // 화면 밖으로 나간 미사일 제거
            }
            else {
                ++it;
            }
        }


        // 적과 미사일 충돌 처리
              for (auto& missile : missiles) {
                for (auto it = cheeseEnemies.begin(); it != cheeseEnemies.end();) {
                    if (missile.isHitByMissile(it->sprite)) {
                        missile.shape.setPosition(-100, -100); // 미사일 제거

                        // 기존 적을 먼저 제거
                        it = cheeseEnemies.erase(it);
                        enemyCnt--;  // 남은 적 갯수 줄어듬
                         //적이 죽으면 새로 생성
                    }
                    else {
                        ++it;
                    }
                }
                // 나머지 적도 같은 방식으로 수정
           

            for (auto it = lettuceEnemies.begin(); it != lettuceEnemies.end();) {
                if (missile.isHitByMissile(it->sprite)) {
                    missile.shape.setPosition(-100, -100);
                    it = lettuceEnemies.erase(it);
                    enemyCnt--;  // 남은 적 갯수 줄어듬
                }
                else {
                    ++it;
                }
            }
            for (auto it = pattyEnemies.begin(); it != pattyEnemies.end();) {
                if (missile.isHitByMissile(it->sprite)) {
                    missile.shape.setPosition(-100, -100);
                    it = pattyEnemies.erase(it);
                    enemyCnt--;
                }
                else {
                    ++it;
                }
            }
            for (auto it = buneEnemies.begin(); it != buneEnemies.end();) {
                if (missile.isHitByMissile(it->sprite)) {
                    missile.shape.setPosition(-100, -100);
                    it = buneEnemies.erase(it);
                    enemyCnt--;
                }
                else {
                    ++it;
              }
            }  
         }

        // 각 적 이동
        for (auto& enemy : cheeseEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }
        for (auto& enemy : pattyEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }
        for (auto& enemy : lettuceEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }
        for (auto& enemy : buneEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            // 각 적 이동
        }

        // 화면 갱신
        window.clear();
        window.draw(backgroundSprite);

        // 남은 적  텍스트 추가
        enemyCountText.setString("Remaining Enemies: " + std::to_string(enemyCnt));  
        window.draw(enemyCountText);

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
        for (auto& missile : missiles)
            window.draw(missile.shape);
        window.draw(player.sprite);
      
        window.display();
    }


    return 0;
}
