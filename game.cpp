#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// ȭ�� ũ�� ����
const int WINDOW_WIDTH = 1300;  
const int WINDOW_HEIGHT = 800;
const int FLOOR_COUNT = 4;   // �� ����
const float GRAVITY = 0.5f;
const float JUMP_FORCE = -15.0f;
const float DOWN_FORCE = 10.0f;
const float FLOOR_SPACING = 200.0f;  // �� �� ������ �ٿ��� ȭ�鿡 �� ������ ����
const float FLOOR_THICKNESS = 40.0f;  // �� �β� ����
int lastDirection = 1;  // 1: ������, -1: ���� (�⺻���� ������)   
 int enemyCnt = 50;

class Player {
public:
    sf::Sprite sprite;
    sf::Texture texture;
    float velocityY = 0.0f;
    bool isJumping = false;
    bool isDown = false;
    int hp = 3;             // �÷��̾� ü��
    float moveSpeed = 10.0f; // �÷��̾� �̵� �ӵ�

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
        lastDirection = -1; // �������� �̵�
    }

    void moveRight() {
        sprite.move(moveSpeed, 0);
        lastDirection = 1; // ���������� �̵�
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

// �� ���� ���� �� Ŭ������
// PattyEnemy Ŭ����
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
        sprite.setPosition(x, y-50);  //y�� ���� (������ٵ�� ���� ����� �޶�)
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
        if (splitClock.getElapsedTime().asSeconds() >=7) {
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
            leaf.sprite.setPosition(sprite.getPosition().x + i * 40, sprite.getPosition().y+50); // �� ��ġ �и�
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

    BuneEnemy(const std::string& textureFile, float x, float y, float speed)
        : speed(speed), groundLevel(y) {  // y ��ġ�� ������ �ٴ��� Y ��ǥ
        if (!texture.loadFromFile(textureFile)) {
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



class Missile {
public:
    sf::CircleShape shape;
    float speed;
    int direction; // �̻����� ���� (-1: ����, 1: ������)

    // ������: �̻��� ���� �ʱ�ȭ
    Missile(float x, float y, float speed, int direction)
        : speed(speed), direction(direction) {
        shape.setRadius(10.0f); // �̻��� ũ��
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y + 70.0f); // y���� �����Ͽ� �߻� ��ġ ����
    }

    // �̻��� �̵�
    void move() {
        shape.move(speed * direction, 0); // �̻��� ���⿡ ���� �̵�
    }

    // ȭ�� ������ �������� Ȯ��
    bool isOffScreen() {
        return shape.getPosition().x < 0 || shape.getPosition().x > WINDOW_WIDTH;
    }

    // ���� �浹�ߴ��� Ȯ��
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
    const float FLOOR_OFFSET = 150.0f;
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;
        floors.emplace_back(0, y, WINDOW_WIDTH, FLOOR_THICKNESS);
        floorPositions.push_back(y);
    }

    // �� ���� ������ �� ����
    vector<BuneEnemy> buneEnemies;
    vector<CheeseEnemy> cheeseEnemies;
    vector<LettuceEnemy> lettuceEnemies;
    vector<PattyEnemy> pattyEnemies;

    // 1�� �� �� ����
    buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 70, 2.0f);
    // 2�� ġ�� �� ����
    cheeseEnemies.emplace_back("img/cheese.png", "img/cheesefloor.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);
    // 3�� ���� �� ����
    lettuceEnemies.emplace_back("img/lettuce.png", "img/lettuces.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);
    // 4�� ��Ƽ �� ����
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 5.0f,1);


    // �̻��� 
    vector<Missile> missiles;

    // �̻��� �߻� ���� �ð�
    sf::Clock missileClock;
    const float MISSILE_COOLDOWN = 1.0f; //1�� 
    
    //���� �� �ؽ�Ʈ�� ����
        sf::Font font;
        if (!font.loadFromFile("fonts/MaplestoryBold.ttf")) {
            std::cerr << "Failed to load font!" << std::endl;
            return -1;
        }
        //���� �� �ؽ�Ʈ ����
        sf::Text enemyCountText;
        enemyCountText.setFont(font);
        enemyCountText.setCharacterSize(50);
        enemyCountText.setFillColor(sf::Color::Red);
        enemyCountText.setPosition(10, 60);

        // �� �߰� Ÿ�̸� ����
        sf::Clock enemyClock; // ���� 5�ʸ��� ������ Ÿ�̸�

  // ���� ����
  while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // �÷��̾� �̵�
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && player.sprite.getPosition().x > 0)
            player.moveLeft(); // �� �Լ� ȣ��� ����
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) &&
            player.sprite.getPosition().x + player.sprite.getGlobalBounds().width < WINDOW_WIDTH)
            player.moveRight(); // �� �Լ� ȣ��� ����
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.jump();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.moveDown();


  
        player.applyGravity(); // �߷� ����  
        player.checkCollision(floorPositions);// �浹 Ȯ��

        // ���� �÷��̾� �浹 ó��
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


        //�����̽� �������� �̻��� �߻� 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (missileClock.getElapsedTime().asSeconds() > MISSILE_COOLDOWN) {
                // �̻��� ���� �� �÷��̾��� ��ġ�� ������ ����
                missiles.emplace_back(
                    player.sprite.getPosition().x + player.sprite.getGlobalBounds().width / 2,
                    player.sprite.getPosition().y,
                    10.0f, // �̻��� �ӵ�
                    lastDirection // �÷��̾��� ������ ���� ����
                );
                missileClock.restart();
            }
        }


        // �̻��� �̵�
        for (auto it = missiles.begin(); it != missiles.end();) {
            it->move(); // �� �̻����� �ڽ��� �������� �̵�
            if (it->isOffScreen()) {
                it = missiles.erase(it); // ȭ�� ������ ���� �̻��� ����
            }
            else {
                ++it;
            }
        }


        // ���� �̻��� �浹 ó��
              for (auto& missile : missiles) {
                for (auto it = cheeseEnemies.begin(); it != cheeseEnemies.end();) {
                    if (missile.isHitByMissile(it->sprite)) {
                        missile.shape.setPosition(-100, -100); // �̻��� ����

                        // ���� ���� ���� ����
                        it = cheeseEnemies.erase(it);
                        enemyCnt--;  // ���� �� ���� �پ��
                         //���� ������ ���� ����
                    }
                    else {
                        ++it;
                    }
                }
                // ������ ���� ���� ������� ����
           

            for (auto it = lettuceEnemies.begin(); it != lettuceEnemies.end();) {
                if (missile.isHitByMissile(it->sprite)) {
                    missile.shape.setPosition(-100, -100);
                    it = lettuceEnemies.erase(it);
                    enemyCnt--;  // ���� �� ���� �پ��
                }
                else {
                    ++it;
                }
            }
            for (auto it = pattyEnemies.begin(); it != pattyEnemies.end();) {
                if (missile.isHitByMissile(it->sprite)) {
                    missile.shape.setPosition(-100, -100);
                    it->hp--;
                    if (it->hp == 0) {   //��Ƽ���� ü���� 0�϶�
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

               // ���� ���ʸ��� ����
        if (enemyClock.getElapsedTime().asSeconds() >= 3.0f && enemyCnt > 0) {
            // �� ���� ���� �߰�
            if (buneEnemies.size() < 1 && enemyCnt > 0) {
                buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH ), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 70, 2.0f);
             
            }
            if (cheeseEnemies.size() < 1 && enemyCnt > 0) {
                cheeseEnemies.emplace_back("img/cheese.png","img/cheesefloor.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);
             
            }
            if (lettuceEnemies.size() < 1 && enemyCnt > 0) {
                lettuceEnemies.emplace_back("img/lettuce.png","img/lettuces.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);
          
            }
            // �ݹ� Ȯ���� Ʈ������Ƽ & �Ϲ� ��Ƽ ����
            if (pattyEnemies.size() < 1 && enemyCnt > 0) {
                // 50% Ȯ���� Ʈ���� ��Ƽ �Ǵ� �Ϲ� ��Ƽ ����
                if (std::rand() % 2 == 0) { // 50% Ȯ��
                    pattyEnemies.emplace_back("img/patty3.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 150, 5.0f, 3);// ü��: 3 (Ʈ���� ��Ƽ)
                }
                else {
                    pattyEnemies.emplace_back( // �Ϲ� ��Ƽ
                     "img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70,
                        5.0f,1 ); // ü��: 1 (�Ϲ� ��Ƽ)
                }
            }
            // Ÿ�̸� ����
            enemyClock.restart();
        }

        // �� �� �̵�
        for (auto& enemy : cheeseEnemies) {
            enemy.move(0, WINDOW_WIDTH);      // �̵�
            enemy.spawnCheeseFloor();        // ġ�� �ٴ� ����
            enemy.updateCheeseFloors();      // ������ ġ�� �ٴ� ����
        }
        for (auto& enemy : pattyEnemies) {
            enemy.move(0, WINDOW_WIDTH);
        }
        for (auto& enemy : lettuceEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            // ���� ó�� (5�ʸ��� �и�, ��ġ��)
            enemy.attack();
        }
        for (auto& enemy : buneEnemies) {
            enemy.move(0, WINDOW_WIDTH);
            enemy.jump();
            // �� �� �̵�
        }

        // ȭ�� ����
        window.clear();
        window.draw(backgroundSprite);

        // ���� ��  �ؽ�Ʈ �߰�
        enemyCountText.setString("Remaining Enemies: " + std::to_string(enemyCnt));  
        window.draw(enemyCountText);

        for (auto& floor : floors)
            window.draw(floor.shape);
        for (auto& enemy : buneEnemies)
            window.draw(enemy.sprite);
        for (auto& enemy : cheeseEnemies)
            window.draw(enemy.sprite);
        // ġ�� �ٴ� ������
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
