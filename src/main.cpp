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

int paddleX = 5;
int paddleY = 20;
const int paddleWidth = 2;
const int paddleHeight = 15;
const int paddleSpeed = 2;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup()
{
  Wire.begin(SDA_PIN, SCL_PIN);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.fillScreen(0);
  display.display();
}

void loop()
{
  display.fillRect(paddleX, paddleY, paddleWidth, paddleHeight, 1);
  display.display();
}
