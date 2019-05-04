// Code written by Bordeanu Calin

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <bits/stdc++.h>

#define WIDTH  1000
#define HEIGHT 800

#define PI 3.14159

sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Space Shooter", sf::Style::Close);

/*
    The way my animation class works is by loading up a picture that consists of N sprites
    After the picture loads, it divides it in N rectangles representing the space that each sprite occupies.
    Those rectangles are put into a vector(std::vector<IntRect>), and are played by indexing through the vector
    and setting the current sprite that will be rendered to the rectangle from the image. If the frame goes above
    the size of the vector, it will go back to 0 reseting the animation.
*/

class Animation
{
public:
    sf::Sprite sprite;
    std::vector<sf::IntRect> frames;
    float frame;
    bool playAnim = false;

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
        sprite.setPosition(WIDTH+100, HEIGHT+100);
    }
    void Update(bool PlayOnce, float Speed)
    {
        frame+=Speed;
        int n = frames.size();
        if(frame>=n){
             if(!PlayOnce) frame = 0;
        }
        if(n>0 && frame<n) sprite.setTextureRect(frames[(int)frame]);
    }
    bool DonePlaying()
    {
        if(frame>=frames.size()) return true;
        else return false;
    }
    void PlayOnce(sf::Vector2f Position, float Speed)
    {
        sprite.setPosition(Position);
        Update(true, Speed);
        if(DonePlaying()){
            sprite.setPosition(HEIGHT+100, WIDTH+100);
        }
    }
};

class Laser
{
private:
    float dx = 0, dy = 0, normalize = 0;
public:
    sf::Sprite sprite;
    float speed;
    bool canBeShot = true;

    //Points the laser ray to the mouse pointer
    void SetRotationAndPos( sf::Vector2f startPos)
    {
        canBeShot = false;

        //Calculating the angle using the inverse of the tangent
        dx = sf::Mouse::getPosition(window).x - startPos.x;
        dy = sf::Mouse::getPosition(window).y - startPos.y;
        float rotation = atan2(dy, dx) * 180.0f/PI + 90;
        //I multiplied the result by 180/PI in order to convert it to degrees
        //and added 90 to it because it was a bit off

        sprite.setRotation(rotation);
        sprite.setPosition(startPos);
    }

    //Hides the laser from the screen and makes it available to shoot
    void AddToPool()
    {
        canBeShot = true;
        sprite.setPosition(WIDTH+100, HEIGHT+100);
    }
    //Initializes the laser by setting it's texture and speed and by adding it to the pool
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
            //Moving the sprite along the normalized vector
            normalize = sqrt((dx*dx)+(dy*dy));
            sprite.move(dx/normalize * speed, dy/normalize * speed);
        }
    }
    //Checking if the laser went off the screen so we can add it to the pool
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

    //Setting the texture, position, speed and scale
    void Init(sf::Texture &t, float Speed)
    {
        sprite.setTexture(t);
        sprite.setOrigin(t.getSize().x/2, t.getSize().y/2);
        sprite.setScale(0.09f, 0.09f);
        sprite.setPosition(WIDTH/2, HEIGHT/2);

        speed = Speed;
    }

    //Points the player to the crosshair
    void Rotate()
    {
        //Calculating the angle
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

            sprite.move(dx/normalize * speed, dy/normalize * speed); //Moving along the normalized vector
        }
        else{
            //This is what gives that smooth decrease in speed after letting go of 'W'
            if(auxSpeed>0){
                sprite.move(dx/normalize*auxSpeed, dy/normalize*auxSpeed);
                auxSpeed-=smoothness;
            }
        }
    }

    //Teleporting the player to the other side of the screen
    //if he goes off the screen
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
    float x = rand()%WIDTH;
    float y = rand()%HEIGHT;
    float length = sqrt(x*x + y*y);

    void Move()
    {
        sprite.move(x/length * speed, y/length * speed); //Moving along the normalized vector
    }
    void Init(float Speed, sf::Texture &texture)
    {
        //Random start direction
        int negPoz = rand()%4;
        if(negPoz == 0) x=-x;
        if(negPoz == 1) y=-y;
        if(negPoz == 2){
            x=-x;
            y=-y;
        }
        length = sqrt(x*x + y*y);

        speed = rand()%(int)Speed + 3; //This gives us some variation in speed for every asteroid

        sprite.setOrigin(768/2, 829/2);
        sprite.setTexture(texture);
        sprite.setPosition(WIDTH/2, HEIGHT/2);
        sprite.setScale(0.04f, 0.04f);
    }
    //Checks if the asteroid hit the wall
    //In case it did, we reverse the x and y coordinates
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
        sprite.setPosition(WIDTH+100, HEIGHT+100);
    }
};

//More accurate collision detection by calculating the distance between 2 circles
bool CircleCollisionCheck(sf::Sprite a, float Radius_A, sf::Sprite b, float Radius_B)
{
    //Getting the positions of the sprites
    sf::Vector2f posA = a.getPosition();
    sf::Vector2f posB = b.getPosition();

    //Calculating the distance between the 2 sprite positions
    float distance = sqrt((posA.x - posB.x) * (posA.x - posB.x) + (posA.y - posB.y)* (posA.y - posB.y));
    //If the distance between those 2 sprites is less than or equal to the sum of the radii it means that they're colliding
    if(distance <= Radius_A + Radius_B) return true;
    return false;
}

int main()
{
    srand(time(0)); // Seed for random number generator
    window.setFramerateLimit(60);
    window.setMouseCursorVisible(false);

    //Loading the textures
    sf::Texture spaceshipTexture, crosshairTexture, laserTexture, asteroidTexture, explosionTexture;
    spaceshipTexture.loadFromFile("textures/spaceship.png");
    crosshairTexture.loadFromFile("textures/crosshair.png");
    laserTexture.loadFromFile("textures/laser.png");
    asteroidTexture.loadFromFile("textures/asteroid.png");
    explosionTexture.loadFromFile("textures/explosion.png");

    sf::Sprite crosshair;
    crosshair.setTexture(crosshairTexture);
    crosshair.setScale(0.05f, 0.05f);
    crosshair.setOrigin(512/2, 512/2);

    Player player;
    player.Init(spaceshipTexture, 10.f);
    sf::Color defaultPlayerTint = player.sprite.getColor();

    //Initializing laser rays
    std::vector<Laser> laserRays(20);
    for(int i = 0; i<laserRays.size(); i++) {laserRays[i].Init(20.f, laserTexture);}


    //Initializing and spawning asteroids and explosions
    std::vector<Asteroid> asteroids(15);
    std::vector<Animation> asteroidExplosions(15);
    for(int i = 0; i<asteroids.size(); i++) {
        asteroids[i].Init(6.f, asteroidTexture);
        asteroidExplosions[i].Init(explosionTexture, 4);
        asteroidExplosions[i].sprite.setScale(0.2f, 0.2f);
    }

    // Timer for the player shield in the beginning
    sf::Clock ShieldTime;
    bool measureTime = true;

    //Animations
    Animation explosionAnimPlayer;
    explosionAnimPlayer.sprite.setScale(0.7f, 0.7f);
    explosionAnimPlayer.Init(explosionTexture, 4);


    // UI
    sf::Font arial;
    arial.loadFromFile("fonts/arial.ttf");

    sf::Text endText;
    endText.setFont(arial);
    endText.setCharacterSize(44);
    endText.setFillColor(sf::Color::Black);
    endText.setString("\tGame Over\nPress R to restart");
    endText.setPosition(WIDTH/2 - endText.getLocalBounds().width/2.f, HEIGHT/3 - endText.getLocalBounds().height/2.f);

    //Sounds
    sf::SoundBuffer laserBuffer, playerExplosionBuffer, asteroidExplosionBuffer;
    laserBuffer.loadFromFile("sounds/laser.wav");
    playerExplosionBuffer.loadFromFile("sounds/spaceship-explosion.wav");
    asteroidExplosionBuffer.loadFromFile("sounds/asteroid-explosion.wav");

    sf::Sound laserSound, playerExplosionSound, asteroidExplosionSound;
    laserSound.setBuffer(laserBuffer);
    playerExplosionSound.setBuffer(playerExplosionBuffer);
    asteroidExplosionSound.setBuffer(asteroidExplosionBuffer);

    bool wonGame = false;

    while (window.isOpen())
    {
        window.clear(sf::Color(100, 100, 100));
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed || sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
                window.close();
            if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && event.type == sf::Event::MouseButtonPressed && !player.frozen){
                for(int i = 0; i<laserRays.size(); i++){
                    if(laserRays[i].canBeShot==true && !player.dead){
                        laserRays[i].SetRotationAndPos(player.sprite.getPosition());
                        laserSound.play();
                        break;
                    }
                }
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::R) && event.type == sf::Event::KeyPressed && (wonGame || player.dead)){
                 main();
            }
        }
        crosshair.setPosition((sf::Vector2f)sf::Mouse::getPosition(window)); //Moves the crosshair with the mouse

        if(ShieldTime.getElapsedTime().asSeconds()>1 && measureTime) {player.frozen = false; measureTime = false;} //Shield in the beginning
        else player.sprite.setColor(sf::Color(120, 120, 120));

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
           explosionAnimPlayer.PlayOnce(player.sprite.getPosition(), 0.2f);
           window.draw(endText);
        }

        wonGame = true;
        //Checking for collision
        for(int i = 0; i<asteroids.size(); i++){
            if(!asteroids[i].dead) wonGame = false;

            for(int j = 0; j<laserRays.size(); j++){
                if(asteroids[i].sprite.getGlobalBounds().intersects(laserRays[j].sprite.getGlobalBounds()) && !asteroids[i].dead){
                    laserRays[j].AddToPool();
                    asteroidExplosionSound.play();
                    asteroids[i].dead = true;
                    break;
                }
            }
            if(!asteroids[i].dead){
                asteroids[i].Move();
                asteroids[i].ChangeDirection();
            }
            else{
                asteroidExplosions[i].PlayOnce(asteroids[i].sprite.getPosition(), 0.2f);
                if(asteroidExplosions[i].DonePlaying()) asteroids[i].AddToPool();
            }
            if(CircleCollisionCheck(player.sprite, 20, asteroids[i].sprite, 5) && !player.frozen && !player.dead && !asteroids[i].dead){
                asteroids[i].AddToPool();
                playerExplosionSound.play();
                player.dead = true;
            }
        }

        if(wonGame){
            endText.setString("\tYou won\nPress R to restart");
            window.draw(endText);
        }

        //Drawing stuff to the window
        for(int i = 0; i<asteroids.size(); i++) {
            window.draw(asteroids[i].sprite);
            window.draw(asteroidExplosions[i].sprite);
        }
        for(int i = 0; i<laserRays.size(); i++) window.draw(laserRays[i].sprite);

        window.draw(player.sprite);
        window.draw(explosionAnimPlayer.sprite);
        window.draw(crosshair);
        window.display();
    }
}
