#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define PAUSE 1000

#define SDA_PIN 33
#define SCL_PIN 32

const int potPin = 14;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int paddleWidth = 3;
const int paddleHeight = 12;
int paddleX = 8;
int paddleY = 24;
int cpuPaddleX = 123;
float cpuPaddleY = 24;
float cpuSpeed = 0.8;

int playerScore = 0;
int cpuScore = 0;

const float ballSize = 1;
float ballX = 64;
float ballY = 20;
float ballSpeedX = 1.6;
float ballSpeedY = 1.1;

void drawGame()
{
  display.clearDisplay();
  display.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, 1);
  display.fillRect(cpuPaddleX, cpuPaddleY, paddleWidth, paddleHeight, 1);
  display.fillCircle(ballX, ballY, ballSize, 1);

  for (int i = 2; i < SCREEN_HEIGHT - 2; i += 8)
  {
    display.fillRect((SCREEN_WIDTH / 2) - 1, i, 2, 4, 1);
  }

  display.setTextColor(1);
  display.setTextSize(2);
  display.setCursor(SCREEN_WIDTH / 2 - 20, 2);
  display.print(playerScore);

  display.setCursor(SCREEN_WIDTH / 2 + 10, 2);
  display.print(cpuScore);

  display.display();
}

void resetBall(int loser)
{
  ballX = 64;
  ballY = 20;

  int direction = (loser == 0) ? 1 : -1;
  ballSpeedX = direction * 1.6;
  ballSpeedY = (random(0, 2) == 0 ? 1 : -1) * 1.1;
}

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();
}

void loop()
{
  paddleY = map(analogRead(potPin), 0, 4095, 0, SCREEN_HEIGHT - paddleHeight);
  paddleY = constrain(paddleY, 0, SCREEN_HEIGHT - paddleHeight);

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Only move when ball is on the right of the screen
  if (ballX >= (SCREEN_WIDTH / 2))
  {
    // Track ball center vs paddle center
    float cpuPaddleCenter = cpuPaddleY + (paddleHeight / 2);
    if (ballY > cpuPaddleCenter)
    {
      cpuPaddleY += 0.8; // move down
    }
    else if (ballY < cpuPaddleCenter)
    {
      cpuPaddleY -= 0.8; // move up
    }
  }
  cpuPaddleY = constrain(cpuPaddleY, 0, SCREEN_HEIGHT - paddleHeight);

  // Wall collision Y bounce ball back
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT)
  {
    ballSpeedY *= -1;
  }

  // Player paddle collision
  if (ballX - ballSize <= paddleX + paddleWidth)
  {
    // Check if ball Y is between the paddle
    if (ballY >= paddleY && ballY <= paddleY + paddleHeight)
    {
      ballSpeedX *= -1.2;
      ballX = paddleX + paddleWidth + ballSize; // Push the ball off the paddle
    }
  }

  // CPU paddle collision
  if (ballX + ballSize >= cpuPaddleX && ballX < cpuPaddleX + paddleWidth)
  {
    if (ballY >= cpuPaddleY && ballY <= cpuPaddleY + paddleHeight)
    {
      ballSpeedX *= -1;
      ballX = cpuPaddleX - ballSize;
    }
  }

  if (ballX <= 0)
  {
    cpuScore++;
    resetBall(0);
  }
  else if (ballX >= SCREEN_WIDTH)
  {
    playerScore++;
    resetBall(1);
  }

  drawGame();
}
