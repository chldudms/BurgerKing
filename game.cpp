#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// 화면 크기 정의
const int WINDOW_WIDTH = 1300;  
const int WINDOW_HEIGHT = 800;
const int FLOOR_COUNT = 4;   // 층 개수
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
    float moveSpeed = 10.0f; // 플레이어 이동 속도

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
// PattyEnemy 클래스
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
        sprite.move(speed , 0);

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
    LettuceEnemy(const std::string& textureFile, const std::string& leafTextureFile, float x, float y, float speed) : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load LettuceEnemy texture!" << std::endl;
            exit(-1);
        }
        if (!leafTexture.loadFromFile(leafTextureFile)) {
            std::cerr << "Failed to load LettuceLeaf texture!" << std::endl;
            exit(-1);
        }
        sprite.setTexture(texture);
        sprite.setPosition(x, y-50);  //y값 위로 (양상추잎들과 사진 사이즈가 달라서)
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
        if (splitClock.getElapsedTime().asSeconds() >=7) {
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
            leaf.sprite.setPosition(sprite.getPosition().x + i * 40, sprite.getPosition().y+50); // 잎 위치 분리
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

    CheeseEnemy(const std::string& textureFile, const std::string& floorTextureFile, float x, float y, float speed)
        : speed(speed) {
        if (!texture.loadFromFile(textureFile)) {
            std::cerr << "Failed to load CheeseEnemy texture!" << std::endl;
            exit(-1);
        }
        if (!cheeseFloorTexture.loadFromFile(floorTextureFile)) {
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

    BuneEnemy(const std::string& textureFile, float x, float y, float speed)
        : speed(speed), groundLevel(y) {  // y 위치는 착지할 바닥의 Y 좌표
        if (!texture.loadFromFile(textureFile)) {
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
        shape.setPosition(x, y + 70.0f); // y값을 조정하여 발사 위치 설정
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
    const float FLOOR_OFFSET = 150.0f;
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;
        floors.emplace_back(0, y, WINDOW_WIDTH, FLOOR_THICKNESS);
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
    cheeseEnemies.emplace_back("img/cheese.png", "img/cheesefloor.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);
    // 3층 상추 적 생성
    lettuceEnemies.emplace_back("img/lettuce.png", "img/lettuces.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);
    // 4층 패티 적 생성
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 5.0f,1);


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

        // 적 추가 타이머 설정
        sf::Clock enemyClock; // 적을 5초마다 생성할 타이머

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


  
        player.applyGravity(); // 중력 적용  
        player.checkCollision(floorPositions);// 충돌 확인

        // 적과 플레이어 충돌 처리
        for (auto& enemy : buneEnemies) {
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                player.takeDamage();
            }
        }
        for (auto& enemy : cheeseEnemies) { 
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds()) || enemy.checkPlayerCollision(player.sprite)) {
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
                    10.0f, // 미사일 속도
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
                    it->hp--;
                    if (it->hp == 0) {   //패티적은 체력이 0일때
                        it = pattyEnemies.erase(it);  enemyCnt--;
                    }
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

               // 적을 ３초마다 생성
        if (enemyClock.getElapsedTime().asSeconds() >= 3.0f && enemyCnt > 0) {
            // 각 층에 적을 추가
            if (buneEnemies.size() < 1 && enemyCnt > 0) {
                buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH ), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 70, 2.0f);
             
            }
            if (cheeseEnemies.size() < 1 && enemyCnt > 0) {
                cheeseEnemies.emplace_back("img/cheese.png","img/cheesefloor.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);
             
            }
            if (lettuceEnemies.size() < 1 && enemyCnt > 0) {
                lettuceEnemies.emplace_back("img/lettuce.png","img/lettuces.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);
          
            }
            // 반반 확률로 트리플패티 & 일반 패티 생성
            if (pattyEnemies.size() < 1 && enemyCnt > 0) {
                // 50% 확률로 트리플 패티 또는 일반 패티 생성
                if (std::rand() % 2 == 0) { // 50% 확률
                    pattyEnemies.emplace_back("img/patty3.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 150, 5.0f, 3);// 체력: 3 (트리플 패티)
                }
                else {
                    pattyEnemies.emplace_back( // 일반 패티
                     "img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70,
                        5.0f,1 ); // 체력: 1 (일반 패티)
                }
            }
            // 타이머 리셋
            enemyClock.restart();
        }

        // 각 적 이동
        for (auto& enemy : cheeseEnemies) {
            enemy.move(0, WINDOW_WIDTH);      // 이동
            enemy.spawnCheeseFloor();        // 치즈 바닥 생성
            enemy.updateCheeseFloors();      // 오래된 치즈 바닥 제거
        }
        for (auto& enemy : pattyEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }
        for (auto& enemy : lettuceEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            // 공격 처리 (5초마다 분리, 합치기)
            enemy.attack();
        }
        for (auto& enemy : buneEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            enemy.jump();
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
        // 치즈 바닥 렌더링
        for (auto& enemy : cheeseEnemies) 
            enemy.drawCheeseFloors(window);
        for (auto& enemy : lettuceEnemies)
            enemy.draw(window);
        for (auto& enemy : pattyEnemies)
            window.draw(enemy.sprite);
        for (auto& missile : missiles)
            window.draw(missile.shape);
        window.draw(player.sprite);
      
        window.display();
    }


    return 0;
}
