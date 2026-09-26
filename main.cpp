#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>


using namespace sf;

const Keyboard::Key controls[5] = {
    Keyboard::W,
    Keyboard::S,
    Keyboard::Up,
    Keyboard::Down,
    Keyboard::Escape
};
Vector2f ball_velocity;
bool isPlayer1Serving = true;
const float initialVelocityX = 130.f; //horizontal velocity
const float initialVelocityY= 80.f;

const Vector2f paddleSize(25.f, 100.f);
const float ballRadius = 10.f;

const int gameWidth = 800;
const int gameHeight = 600;

const float paddleSpeed = 400.f;
const float paddleOffsetWall = 10.f;
const float time_step = 0.017f;

const float velocityMultiplier = 1.1f;

bool gameWon = false;

int player1Score = 0;
int player2Score = 0;

Font font;
Text text;
Text winningText;

CircleShape ball;
RectangleShape paddles[2];


void reset() {
    paddles[0].setPosition(
        paddleOffsetWall + paddleSize.x / 2.f,
        gameHeight / 2.f
    );

    paddles[1].setPosition(
        gameWidth - paddleOffsetWall - paddleSize.x / 2.f,
        gameHeight / 2.f
    );

    ball.setPosition(
        gameWidth / 2.f,
        gameHeight / 2.f
    );

    ball_velocity = {
        (isPlayer1Serving ? initialVelocityX : -initialVelocityX),
        initialVelocityY
    };

    text.setString(std::to_string(player1Score) + " : " + std::to_string(player2Score));
    // Keep Score Text Centered
    text.setPosition((gameWidth * .5f) - (text.getLocalBounds().width * .5f), 0);
}

void init() {

    for (RectangleShape &p : paddles) {
        p.setSize(paddleSize);
        p.setOrigin(paddleSize/2.f);
    }
    ball.setRadius(ballRadius);
    ball.setOrigin(ballRadius, ballRadius);


    font.loadFromFile("resources/fonts/RobotoMono-Regular.ttf");
    text.setFont(font);
    text.setCharacterSize(24);
    winningText.setCharacterSize(30);
    winningText.setFillColor(sf::Color::Green);
    winningText.setFont(font);

    reset();

}

void update(float dt) {
    if (gameWon) {
        return;
    }
    float leftDirection = 0.0f;
    float rightDirection = 0.0f;

    if (Keyboard::isKeyPressed(controls[0]))
        leftDirection--;

    if (Keyboard::isKeyPressed(controls[1]))
        leftDirection++;

    if (Keyboard::isKeyPressed(controls[2]))
        rightDirection--;

    if (Keyboard::isKeyPressed(controls[3]))
        rightDirection++;

    paddles[0].move({0.f, leftDirection * paddleSpeed * dt});
    paddles[1].move({0.f, rightDirection * paddleSpeed * dt});

    ball.move(ball_velocity * dt);

    // check ball collision
    const float bx = ball.getPosition().x;
    const float by = ball.getPosition().y;
    if (by > gameHeight) { //bottom wall
        // bottom wall
        ball_velocity.x *= velocityMultiplier;
        ball_velocity.y *= -velocityMultiplier;
        ball.move(sf::Vector2f(0.f, -10.f));
    } else if (by < 0) { //top wall
        // top wall
        ball_velocity.x *= velocityMultiplier;
        ball_velocity.y *= -velocityMultiplier;
        ball.move(sf::Vector2f(0.f, 10.f));
    }
    else if (bx > gameWidth) {
        player1Score++;
        reset();
    }
    else if (bx < 0) {
        player2Score++;
        reset();
    }
    else if (
    //ball is inline or behind paddle AND
        bx < paddleSize.x + paddleOffsetWall &&
        //ball is below top edge of paddle AND
        by > paddles[0].getPosition().y - (paddleSize.y * 0.5) &&
        //ball is above bottom edge of paddle
        by < paddles[0].getPosition().y + (paddleSize.y * 0.5)) {
        ball_velocity.x *= -velocityMultiplier;

    }
    else if (
    bx > gameWidth - paddleSize.x - paddleOffsetWall &&

    by > paddles[1].getPosition().y - (paddleSize.y * 0.5f) &&

    by < paddles[1].getPosition().y + (paddleSize.y * 0.5f)) {
        ball_velocity.x *= -velocityMultiplier;

         }
    if (player1Score == 3) {
        gameWon = true;
        winningText.setString(
    "Player 1 Wins!\n"
        "Final Score: "
        + std::to_string(player1Score)
        + " : "
        + std::to_string(player2Score)
        + "\nPress ESC to exit"
);

        // Find the invisible rectangle around the text
        auto bounds = winningText.getLocalBounds();

        // Make the CENTER of that rectangle the origin
        winningText.setOrigin({
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f
        });

        // Put that origin at the center of the window
        winningText.setPosition({
            gameWidth / 2.f,
            gameHeight / 2.f
        });
    }
    else if (player2Score == 3) {
        gameWon = true;
        winningText.setString(
    "Player 2 Wins!\n"
        "Final Score: "
        + std::to_string(player2Score)
        + " : "
        + std::to_string(player1Score)
        + "\nPress ESC to exit"
    );

        // Find the invisible rectangle around the text
        auto bounds = winningText.getLocalBounds();

        // Make the CENTER of that rectangle the origin
        winningText.setOrigin({
            bounds.left + bounds.width / 2.f,
            bounds.top + bounds.height / 2.f
        });

        // Put that origin at the center of the window
        winningText.setPosition({
            gameWidth / 2.f,
            gameHeight / 2.f
        });
    }
}

void render(RenderWindow &window) {

    if (gameWon) {
        // ONLY draw the game-over screen
        window.draw(winningText);
    }
    else {
        // ONLY draw the game while playing
        window.draw(paddles[0]);
        window.draw(paddles[1]);
        window.draw(ball);
        window.draw(text);
    }
}

int main () {

    //create the window
    RenderWindow window(VideoMode({gameWidth, gameHeight}),"PONG");
    //initialise and load
    init();
    while(window.isOpen()){
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            if (Keyboard::isKeyPressed(controls[4])) {
                window.close();
            }
        }
        //Calculate dt
        static Clock clock;
        const float dt = clock.restart().asSeconds();

        window.clear();
        update(dt);

        render(window);
        //wait for the time_step to finish before displaying the next frame.
        sleep(seconds(time_step));
        //Wait for Vsync
        window.display();
    }
}