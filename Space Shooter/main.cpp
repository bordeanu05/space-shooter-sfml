// Code written by Bordeanu Calin

#include <SFML/Graphics.hpp>
#include <bits/stdc++.h>

using namespace std;

#define WIDTH  1000
#define HEIGHT 800

#define PI 3.14159

class Animation
{
public:
    sf::Sprite sprite;
    vector<sf::IntRect> frames;
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

    void SetRotationAndPos(sf::RenderWindow &window, sf::Vector2f startPos)
    {
        canBeShot = false;
        dx = sf::Mouse::getPosition(window).x - startPos.x;
        dy = sf::Mouse::getPosition(window).y - startPos.y;

        float tan = dy/dx;
        float rotation = atan2(dy, dx) * 180.0f/PI + 90;

        sprite.setRotation(rotation);
        sprite.setPosition(startPos);
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
            canBeShot = true;
            sprite.setPosition(WIDTH+10, HEIGHT+10);
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

    void Init(sf::Texture &t, float Speed)
    {
        sprite.setTexture(t);
        sprite.setOrigin(t.getSize().x/2, t.getSize().y/2);
        speed = Speed;
    }

    void Rotate(sf::RenderWindow &window)
    {
        float dx = sf::Mouse::getPosition(window).x - sprite.getPosition().x;
        float dy = sf::Mouse::getPosition(window).y - sprite.getPosition().y;

        float tan = dy/dx;
        float rotation = atan2(dy, dx) * 180.0f/PI+90;

        sprite.setRotation(rotation);
    }

    void Move(sf::RenderWindow &window, float smoothness)
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
};

class Asteroid
{
public:
    sf::Sprite sprite;
    float speed;
    bool dead = false;
    float x = x = rand()%WIDTH;
    float y = y = rand()%HEIGHT;
    float length = sqrt(x*x + y*y);

    void Move()
    {
        sprite.move(x/length * speed, y/length * speed);
    }

    void ChangeDirection()
    {
        int posX = sprite.getPosition().x;
        int posY = sprite.getPosition().y;

        if(posX>WIDTH-20 || posX<20) x = -x;
        if(posY>HEIGHT-20 || posY<20) y = -y;

        length = sqrt(x*x + y*y);
    }
};


int main()
{
    srand(time(0));
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "", sf::Style::Close);
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

    Player player;
    player.Init(soaceshipTexture, 10);
    player.sprite.setPosition(WIDTH/2, HEIGHT/2);
    player.sprite.setScale(0.09f, 0.09f);

    std::vector<Laser> laserRays(20);
    for(int i = 0; i<laserRays.size(); i++){
        laserRays[i].sprite.setTexture(laserTexture);
        laserRays[i].sprite.setScale(0.02f, 0.02f);
        laserRays[i].sprite.setOrigin(100, 300);
        laserRays[i].sprite.setPosition(WIDTH+10, HEIGHT+10);
        laserRays[i].speed = 20.f;
    }

    std::vector<Asteroid> asteroids(15);
    for(int i = 0; i<asteroids.size(); i++){
        asteroids[i].speed = 5;
        asteroids[i].sprite.setOrigin(768/2, 829/2);
        asteroids[i].sprite.setTexture(asteroidTexture);
        asteroids[i].sprite.setPosition(WIDTH/2, HEIGHT/2);
        asteroids[i].sprite.setScale(0.04f, 0.04f);
    }


    while (window.isOpen())
    {
        window.clear(sf::Color::White);
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && event.type == sf::Event::MouseButtonPressed){
                for(int i = 0; i<laserRays.size(); i++){
                    if(laserRays[i].canBeShot==true){
                        laserRays[i].SetRotationAndPos(window, player.sprite.getPosition());
                        break;
                    }
                }
            }
        }
        crosshair.setPosition((sf::Vector2f)sf::Mouse::getPosition(window));

        player.Rotate(window);
        player.Move(window, 0.2f);

        for(int i = 0; i<laserRays.size(); i++){
            laserRays[i].Move();
            laserRays[i].CheckPos();
        }
        for(int i = 0; i<asteroids.size(); i++){
            for(int j = 0; j<laserRays.size(); j++){
                if(asteroids[i].sprite.getGlobalBounds().intersects(laserRays[j].sprite.getGlobalBounds())){
                    asteroids[i].dead = true;
                    asteroids[i].sprite.setPosition(WIDTH+10, HEIGHT+10);
                    break;
                }
            }
            if(!asteroids[i].dead){
                asteroids[i].Move();
                asteroids[i].ChangeDirection();
            }
        }

        for(int i = 0; i<asteroids.size(); i++) window.draw(asteroids[i].sprite);
        for(int i = 0; i<laserRays.size(); i++) window.draw(laserRays[i].sprite);

        //PlayAnim(explosionAnim, false, 0.2f, window);
        //window.draw(explosionAnim.sprite);
        window.draw(player.sprite);
        window.draw(crosshair);
        window.display();
    }
}
