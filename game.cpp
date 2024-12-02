#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include "Enemy.cpp"
#include "Boss.cpp"

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
int enemyCnt = 1;

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
        shape.setPosition(x, y +50.0f); // y���� �����Ͽ� �߻� ��ġ ����
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
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "BurgerKing");
    window.setFramerateLimit(60);

    // ��� �ؽ�ó �� ��������Ʈ
    sf::Texture backgroundTexture, backgroundTexture2, gameOverTexture, gameClearTexture;
    if (!backgroundTexture.loadFromFile("img/back.jpg") || !backgroundTexture2.loadFromFile("img/back2.jpg")|| !gameClearTexture.loadFromFile("img/clear.jpg")) {
        std::cerr << "Failed to load background textures!" << std::endl;
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
    pattyEnemies.emplace_back("img/patty.png", rand() % (WINDOW_WIDTH - 50), WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 5.0f, 1);

    // ����ŷ ���� ���� 
    Boss boss("img/kingboss.png","img/bacon.png", (WINDOW_WIDTH - 150) / 2, WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 150, 10.0f);

    // ���񺸽� 
    Boss zom1("img/zombie.png", (WINDOW_WIDTH - 150) / 2, WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 180, 5.0f);
    // ���񺸽� 
    Boss zom2("img/zombie.png", (WINDOW_WIDTH - 150) / 2, WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 180, 5.0f);
    // ���񺸽� 
    Boss zom3("img/zombie.png", (WINDOW_WIDTH - 150) / 2, WINDOW_HEIGHT - (4 * FLOOR_SPACING) + FLOOR_OFFSET - 180, 5.0f);


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

    bool isCleared = false;  // ���� Ŭ���� ���� ����
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
            // ��ü�� �浹 Ȯ��
            if (player.sprite.getGlobalBounds().intersects(enemy.sprite.getGlobalBounds())) {
                player.takeDamage();
            }
      // �и��� �ٵ�� �浹 Ȯ��
            for (auto& leaf : enemy.lettuceLeaves) {
                if (player.sprite.getGlobalBounds().intersects(leaf.sprite.getGlobalBounds())) {
                    player.takeDamage();
                    break; // �浹�� �߻��ϸ� ���� ������ �浹 Ȯ�� ����
                }
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
                buneEnemies.emplace_back("img/bune.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - FLOOR_SPACING + FLOOR_OFFSET - 70, 2.0f);

            }
            if (cheeseEnemies.size() < 1 && enemyCnt > 0) {
                cheeseEnemies.emplace_back("img/cheese.png", "img/cheesefloor.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - (2 * FLOOR_SPACING) + FLOOR_OFFSET - 120, 3.0f);

            }
            if (lettuceEnemies.size() < 1 && enemyCnt > 0) {
                lettuceEnemies.emplace_back("img/lettuce.png", "img/lettuces.png", rand() % (WINDOW_WIDTH), WINDOW_HEIGHT - (3 * FLOOR_SPACING) + FLOOR_OFFSET - 70, 4.0f);

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
                        5.0f, 1); // ü��: 1 (�Ϲ� ��Ƽ)
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

        for (auto& floor : floors) {
            window.draw(floor.shape);
        }
        for (auto& enemy : buneEnemies) {
            window.draw(enemy.sprite);
        }
        for (auto& enemy : cheeseEnemies) {
            window.draw(enemy.sprite);
        }
        // ġ�� �ٴ� ������
        for (auto& enemy : cheeseEnemies) {
            enemy.drawCheeseFloors(window);
        }
        for (auto& enemy : lettuceEnemies) {
            enemy.draw(window);
        }
        for (auto& enemy : pattyEnemies) {
            window.draw(enemy.sprite);
        }


        for (auto& missile : missiles) {
            window.draw(missile.shape);
        }
        window.draw(player.sprite);

        // ������ ���� ������
        if (enemyCnt == 0) {
            buneEnemies.clear();
            cheeseEnemies.clear();
            lettuceEnemies.clear();
            pattyEnemies.clear();

            backgroundSprite.setTexture(backgroundTexture2);  // ����� back2�� ����

            window.draw(zom1.sprite); // ���� �׸���
            zom1.move(0, WINDOW_WIDTH);
            window.draw(zom2.sprite); // ���� �׸���
            zom2.move(0, WINDOW_WIDTH);
            window.draw(zom3.sprite); // ���� �׸���
            zom3.move(0, WINDOW_WIDTH);

            window.draw(boss.sprite); // ���� �׸���
            boss.move(0, WINDOW_WIDTH);

            for (auto it = missiles.begin(); it != missiles.end(); ) {
                if (it->isHitByMissile(boss.sprite)) {
                    boss.takeDamage(); // ���� ü�� ����
                    std::cout << "Boss HP: " << boss.hp << std::endl; // ������ ���
                    it = missiles.erase(it); // �浹�� �̻��� ����
                    break; // �� ���� �浹 ó�� �� ����
                }
                else {
                    ++it; // ���� �̻��Ϸ� �̵�
                }
            }

            if (boss.hp == 0) {
                isCleared = true;
            }

            if (!isCleared) {
                // ���� �̵� �� ����
                boss.move(0, WINDOW_WIDTH);
                boss.baconShot();
                boss.draw(window);  // ���� �׸���
            }
            // ���� Ŭ���� ����
            if (isCleared) {
                backgroundSprite.setTexture(gameClearTexture);
                window.draw(backgroundSprite);
            }
        }

        window.display();
    }


    return 0;
}
