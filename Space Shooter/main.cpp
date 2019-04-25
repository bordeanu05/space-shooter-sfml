// Code written by Bordeanu Calin

#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>

#define WIDTH  1000
#define HEIGHT 800

#define PI 3.14159

sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "", sf::Style::Close);

class Animation
{
public:
    sf::Sprite sprite;
    std::vector<sf::IntRect> frames;
    float frame;

    void Init(sf::Texture &t, int n)
    {
        int w = t.getSize().x/n;
        int h = t.getSize().y;
        frame = 0;
        for(int i = 0; i<n; i++){
            frames.push_back(sf::IntRect(i*w, 0, w, h));
        }
        sprite.setTexture(t);
        sprite.setOrigin(w/2, h/2);
        sprite.setTextureRect(frames[0]);
    }
    void Update(bool PlayOnce, float speed)
    {
        frame+=speed;
        int n = frames.size();
        if(frame>=n){
             if(!PlayOnce) frame = 0;
        }
        if(n>0 && frame<n) sprite.setTextureRect(frames[(int)frame]);
    }

};

void PlayAnim(Animation &anim, bool PlayOnce, float speed, sf::RenderWindow &window)
{
    anim.Update(PlayOnce, speed);
    window.draw(anim.sprite);
}

class Laser
{
private:
    float dx = 0, dy = 0, normalize = 0;
public:
    sf::Sprite sprite;
    float speed;
    bool canBeShot = true;

    void SetRotationAndPos( sf::Vector2f startPos)
    {
        canBeShot = false;
        dx = sf::Mouse::getPosition(window).x - startPos.x;
        dy = sf::Mouse::getPosition(window).y - startPos.y;

        float rotation = atan2(dy, dx) * 180.0f/PI + 90;

        sprite.setRotation(rotation);
        sprite.setPosition(startPos);
    }
    void AddToPool()
    {
        canBeShot = true;
        sprite.setPosition(WIDTH+10, HEIGHT+10);
    }
    void Init(float Speed, sf::Texture &texture)
    {
        AddToPool();
        sprite.setTexture(texture);
        sprite.setScale(0.02f, 0.02f);
        sprite.setOrigin(100, 300);
        speed = Speed;
    }
    void Move()
    {
        if(!canBeShot){
            normalize = sqrt((dx*dx)+(dy*dy));
            sprite.move(dx/normalize * speed, dy/normalize * speed);
        }
    }
    void CheckPos()
    {
        if(sprite.getPosition().x < -10 || sprite.getPosition().x > WIDTH+10 || sprite.getPosition().y < -10 || sprite.getPosition().y > HEIGHT+10){
            AddToPool();
        }
    }
};

class Player
{
private:
    float dx = 0, dy = 0, normalize = 0, auxSpeed = 0;
public:
    sf::Sprite sprite;
    float speed;
    bool dead = false;
    bool frozen = true;

    void Init(sf::Texture &t, float Speed)
    {
        sprite.setTexture(t);
        sprite.setOrigin(t.getSize().x/2, t.getSize().y/2);
        sprite.setScale(0.09f, 0.09f);
        sprite.setPosition(WIDTH/2, HEIGHT/2);

        speed = Speed;
    }

    void Rotate()
    {
        float dx = sf::Mouse::getPosition(window).x - sprite.getPosition().x;
        float dy = sf::Mouse::getPosition(window).y - sprite.getPosition().y;

        float rotation = atan2(dy, dx) * 180.0f/PI+90;

        sprite.setRotation(rotation);
    }

    void Move(float smoothness)
    {
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            dx = sf::Mouse::getPosition(window).x - sprite.getPosition().x;
            dy = sf::Mouse::getPosition(window).y - sprite.getPosition().y;

            normalize = sqrt((dx*dx)+(dy*dy));
            auxSpeed = speed;

            sprite.move(dx/normalize * speed, dy/normalize * speed);
        }
        else{
            if(auxSpeed>0){
                sprite.move(dx/normalize*auxSpeed, dy/normalize*auxSpeed);
                auxSpeed-=smoothness;
            }
        }
    }
    void CheckBoundaries()
    {
        int posX = sprite.getPosition().x;
        int posY = sprite.getPosition().y;

        if(posX>WIDTH+20) sprite.setPosition(-20, posY);
        if(posX<-20) sprite.setPosition(WIDTH+20, posY);
        if(posY>HEIGHT+20) sprite.setPosition(posX, -20);
        if(posY<-20) sprite.setPosition(posX, HEIGHT+20);
    }
};

class Asteroid
{
public:
    sf::Sprite sprite;
    float speed;
    bool dead = false;
    bool canDestroy = false;
    float x = x = rand()%WIDTH;
    float y = y = rand()%HEIGHT;
    float length = sqrt(x*x + y*y);

    void Move()
    {
        sprite.move(x/length * speed, y/length * speed);
    }
    void Init(float Speed, sf::Texture &texture)
    {
        speed = Speed;
        sprite.setOrigin(768/2, 829/2);
        sprite.setTexture(texture);
        sprite.setPosition(WIDTH/2, HEIGHT/2);
        sprite.setScale(0.04f, 0.04f);
    }
    void ChangeDirection()
    {
        int posX = sprite.getPosition().x;
        int posY = sprite.getPosition().y;

        if(posX>WIDTH-20 || posX<20) x = -x;
        if(posY>HEIGHT-20 || posY<20) y = -y;

        length = sqrt(x*x + y*y);
    }
    void AddToPool()
    {
        dead = true;
        canDestroy = false;
        sprite.setPosition(WIDTH+10, HEIGHT+10);
    }
};

bool CircleCollisionCheck(sf::Sprite a, float Radius_A, sf::Sprite b, float Radius_B)
{
    sf::Vector2f posA = a.getPosition();
    sf::Vector2f posB = b.getPosition();
    float distance = sqrt((posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y)* (posA.y - posB.y));
    if(distance <= Radius_A + Radius_B) return true;
    return false;
}

int main()
{
    srand(time(0));
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    sf::Texture soaceshipTexture, crosshairTexture, laserTexture, asteroidTexture;
    soaceshipTexture.loadFromFile("textures/spaceship.png");
    crosshairTexture.loadFromFile("textures/crosshair.png");
    laserTexture.loadFromFile("textures/laser.png");
    asteroidTexture.loadFromFile("textures/asteroid.png");

    sf::Sprite crosshair;
    crosshair.setTexture(crosshairTexture);
    crosshair.setScale(0.05f, 0.05f);
    crosshair.setOrigin(512/2, 512/2);

    sf::Font arial;
    arial.loadFromFile("fonts/arial.ttf");

    sf::Text deadText;
    deadText.setFont(arial);

    Player player;
    player.Init(soaceshipTexture, 10.f);

    sf::Color defaultPlayerTint = player.sprite.getColor();
    player.sprite.setColor(sf::Color(120, 120, 120));

    std::vector<Laser> laserRays(20);
    for(int i = 0; i<laserRays.size(); i++) {laserRays[i].Init(20.f, laserTexture);}

    std::vector<Asteroid> asteroids(15);
    for(int i = 0; i<asteroids.size(); i++) {asteroids[i].Init(5.f, asteroidTexture);}

    sf::Clock ShieldTime;

    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && event.type == sf::Event::MouseButtonPressed && !player.frozen){
                for(int i = 0; i<laserRays.size(); i++){
                    if(laserRays[i].canBeShot==true){
                        laserRays[i].SetRotationAndPos(player.sprite.getPosition());
                        break;
                    }
                }
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::R)){
                 main();
            }
        }
        crosshair.setPosition((sf::Vector2f)sf::Mouse::getPosition(window));

        if(ShieldTime.getElapsedTime().asSeconds()>1) player.frozen = false;

        if(!player.dead){
            player.Rotate();
            player.CheckBoundaries();
            if(!player.frozen){
                player.Move(0.2f);
                player.sprite.setColor(defaultPlayerTint);
            }

            for(int i = 0; i<laserRays.size(); i++){
                laserRays[i].Move();
                laserRays[i].CheckPos();
            }
        }
        else{
            // death screen
            // explosion anim
            // hide the space ship
        }

        for(int i = 0; i<asteroids.size(); i++){
            for(int j = 0; j<laserRays.size(); j++){
                if(asteroids[i].sprite.getGlobalBounds().intersects(laserRays[j].sprite.getGlobalBounds())){
                    asteroids[i].AddToPool();
                    laserRays[j].AddToPool();
                    break;
                }
            }
            if(!asteroids[i].dead){
                asteroids[i].Move();
                asteroids[i].ChangeDirection();
            }
            if(CircleCollisionCheck(player.sprite, 20, asteroids[i].sprite, 5) && !player.frozen && !player.dead){
                asteroids[i].AddToPool();
                player.dead = true;
            }
        }

        for(int i = 0; i<asteroids.size(); i++) window.draw(asteroids[i].sprite);
        for(int i = 0; i<laserRays.size(); i++) window.draw(laserRays[i].sprite);

        window.draw(player.sprite);
        window.draw(crosshair);
        window.display();
    }
}
