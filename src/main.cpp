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

const int paddleWidth = 2;
const int paddleHeight = 16;
int paddleX = 5;
int paddleY = 24;

const int ballSize = 1;
float ballX = 64;
float ballY = 32;
float ballSpeedX = 1.0;
float ballSpeedY = 0.5;

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

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Check if the ball hit the end of the screen then bounce it back
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT)
  {
    ballSpeedY *= -1;
  }
  else if (ballX >= SCREEN_WIDTH)
  {
    ballSpeedX *= -1;
  }

  // Check paddle x
  if (ballX - ballSize == paddleX + paddleWidth)
  {
    // Check if ball Y is between the paddle
    if (ballY >= paddleY && ballY <= paddleY + paddleHeight)
    {
      ballSpeedX *= -1;
      ballX = paddleX + paddleWidth + ballSize; // Push the ball off the paddle
    }
  }
  else if (ballX <= 0)
  {
    ballX = 64;
    ballY = 20;
    ballSpeedX = 1.0;
    ballSpeedY = 0.5;
  }

  display.clearDisplay();

  display.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, 1);
  display.fillCircle(ballX, ballY, ballSize, 1);

  display.display();
}
