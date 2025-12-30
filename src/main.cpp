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

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int potPin1 = 13;
const int potPin2 = 27;
const int buzzerPin = 26;
const int btnPin = 14;

const int paddleWidth = 3;
const int paddleHeight = 12;
int paddleX = 8;
int paddleY = 24;
int cpuPaddleX = 123;
float cpuPaddleY = 24;
float cpuSpeed = 0.8;

int playerScore = 0;
int cpuScore = 0;

const float ballSize = 2;
float ballX = 64;
float ballY = 20;
float ballSpeedX = 1.0;
float ballSpeedY = 0.5;

int currentState = 0;
bool isTwoPlayer = false;

void soundPaddleHit()
{
  tone(buzzerPin, 440, 50); // 440 Hz for 50ms
}

void soundWallBounce()
{
  tone(buzzerPin, 523, 50); // 523 Hz for 50ms
}

void soundScorePoint()
{
  tone(buzzerPin, 600, 100); // 600 Hz for 100ms
  delay(100);
  noTone(buzzerPin);
  tone(buzzerPin, 800, 150); // 800 Hz for 150ms
}

void resetBall(int loser)
{
  soundScorePoint();
  delay(500);

  ballX = 64;
  ballY = 20;
  int direction = (loser == 0) ? 1 : -1;
  ballSpeedX = direction * 1.0;
  ballSpeedY = (random(0, 2) == 0 ? 1 : -1) * 1.1;
}

void drawMenu()
{
  display.clearDisplay();

  int val = analogRead(potPin1);

  if (val > 2048)
  {
    isTwoPlayer = false;
  }
  else if (val < 2048)
  {
    isTwoPlayer = true;
  }
  Serial.println(val);
  Serial.println(isTwoPlayer);

  display.setTextColor(1);
  display.setTextSize(1);
  display.setCursor(35, 10);
  display.print("SELECT MODE");

  // Selection indicator
  display.setCursor(25, 30);
  display.print(isTwoPlayer ? " VS CPU" : "> VS CPU");
  display.setCursor(25, 45);
  display.print(isTwoPlayer ? "> 2 PLAYER" : " 2 PLAYER");

  if (digitalRead(btnPin) == LOW)
  {
    tone(buzzerPin, 1000, 150);
    playerScore = 0;
    cpuScore = 0;
    currentState = 1;
    delay(200);
  }

  display.display();
}

void drawGame()
{
  display.clearDisplay();
  for (int i = 2; i < SCREEN_HEIGHT - 2; i += 8)
  {
    display.fillRect((SCREEN_WIDTH / 2) - 1, i, 1, 4, 1);
  }
  display.setTextColor(1);
  display.setTextSize(1);
  display.setCursor(SCREEN_WIDTH / 2 - 15, 2);
  display.print(playerScore);

  display.setCursor(SCREEN_WIDTH / 2 + 10, 2);
  display.print(cpuScore);

  display.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, 1);
  display.fillRect(cpuPaddleX, cpuPaddleY, paddleWidth, paddleHeight, 1);
  display.fillCircle(ballX, ballY, ballSize, 1);

  display.display();
}

void startGame()
{
  paddleY = map(analogRead(potPin1), 0, 4095, 0, SCREEN_HEIGHT - paddleHeight);
  paddleY = constrain(paddleY, 0, SCREEN_HEIGHT - paddleHeight);

  if (isTwoPlayer)
  {
    cpuPaddleY = map(analogRead(potPin2), 0, 4095, 0, SCREEN_HEIGHT - paddleHeight);
  }

  cpuPaddleY = constrain(cpuPaddleY, 0, SCREEN_HEIGHT - paddleHeight);

  ballX += ballSpeedX;
  ballY += ballSpeedY;

  // Only move when ball is on the right of the screen
  if (ballX >= (SCREEN_WIDTH / 2) && !isTwoPlayer)
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

  // Wall collision Y bounce ball back
  if (ballY <= 0 || ballY >= SCREEN_HEIGHT)
  {
    ballSpeedY *= -1;
    soundWallBounce();
  }

  // Player paddle collision
  if (ballX - ballSize <= paddleX + paddleWidth)
  {
    // Check if ball Y is between the paddle
    if (ballY >= paddleY && ballY <= paddleY + paddleHeight)
    {
      ballSpeedX *= isTwoPlayer ? -1 : -1.2;
      ballX = paddleX + paddleWidth + ballSize; // Push the ball off the paddle
      soundPaddleHit();
    }
  }

  // CPU paddle collision
  if (ballX + ballSize >= cpuPaddleX && ballX < cpuPaddleX + paddleWidth)
  {
    if (ballY >= cpuPaddleY && ballY <= cpuPaddleY + paddleHeight)
    {
      ballSpeedX *= -1;
      ballX = cpuPaddleX - ballSize;
      soundPaddleHit();
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

void setup()
{
  Serial.begin(115200);
  Wire.begin(SDA_PIN, SCL_PIN);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();
  display.display();

  pinMode(btnPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  delay(500);
}

void loop()
{
  if (currentState == 0)
  {
    drawMenu();
  }
  else if (currentState == 1)
  {
    startGame();
    Serial.println("Game started");
    Serial.println(isTwoPlayer);
  }
}
