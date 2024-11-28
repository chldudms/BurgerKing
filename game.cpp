#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>

using namespace std;

// ȭ�� ũ�� ����
const int WINDOW_WIDTH = 1300;
const int WINDOW_HEIGHT = 800;
const int FLOOR_COUNT = 4;
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
    float moveSpeed = 5.0f; // �÷��̾� �̵� �ӵ�

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

        // ���� �ε����� ���� ����
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

        // ���� �ε����� ���� ����
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
    int direction; // �̻����� ���� (-1: ����, 1: ������)

    // ������: �̻��� ���� �ʱ�ȭ
    Missile(float x, float y, float speed, int direction)
        : speed(speed), direction(direction) {
        shape.setRadius(10.0f); // �̻��� ũ��
        shape.setFillColor(sf::Color::Red);
        shape.setPosition(x, y + 60.0f); // y���� �����Ͽ� �߻� ��ġ ����
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
    const float FLOOR_OFFSET = 150.0f;  // �� ������ ����
    for (int i = 0; i < FLOOR_COUNT; i++) {
        float y = WINDOW_HEIGHT - (i + 1) * FLOOR_SPACING + FLOOR_OFFSET;  // �� �� ������ �ٿ��� ������ ��ġ
        floors.emplace_back(0, y, WINDOW_WIDTH, FLOOR_THICKNESS);  // �� �β� ����
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
    cheeseEnemies.emplace_back("img/cheese.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);
    // 3�� ���� �� ����
    lettuceEnemies.emplace_back("img/lettuce.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);
    // 4�� ��Ƽ �� ����
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 5.0f);


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


   // �߷� ����  // �浹 Ȯ��
        player.applyGravity();
        player.checkCollision(floorPositions);

        // ���� �÷��̾� �浹 ó��
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


        //�����̽� �������� �̻��� �߻� 
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (missileClock.getElapsedTime().asSeconds() > MISSILE_COOLDOWN) {
                // �̻��� ���� �� �÷��̾��� ��ġ�� ������ ����
                missiles.emplace_back(
                    player.sprite.getPosition().x + player.sprite.getGlobalBounds().width / 2,
                    player.sprite.getPosition().y,
                    8.0f, // �̻��� �ӵ�
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

        // �� �� �̵�
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
