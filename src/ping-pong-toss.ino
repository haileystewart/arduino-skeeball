#include <MD_MAX72xx.h>
#include <SPI.h>
#include "sensor.h"
#include "tone.h"

// game defines
#define ULTRASONIC_SENSORS  6
#define DEBUG_SENSOR        -1
#define WAIT_BETWEEN_BALLS  1000
#define BUTTON_PIN          8

// led matrix definitions
#define HARDWARE_TYPE       MD_MAX72XX::FC16_HW
#define DATA_PIN            11
#define CLK_PIN             13
#define CS_PIN              10
#define LED_MODULES         8   // there are 8 8x8 modules
#define LED_BRIGHTNESS      1   // brightness level is 0..15

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, LED_MODULES);

Tone gameTone;

int  score        = 0;
int  highScore    = 0;
int  balls        = 9;
bool gameRunning  = false;
int  cycle        = 0;
int  cycleDelay   = 0;

// list of ultrasonic sensors
Sensor sensors[ULTRASONIC_SENSORS] =
{
  Sensor(A5, 2, 300, 25, 110),
  Sensor(A4, 3, 300, 100, 220),
  Sensor(A4, 3, 300, 100, 220),
  Sensor(A4, 3, 300, 100, 220),
  Sensor(A4, 3, 300, 100, 220),
  Sensor(A4, 3, 300, 100, 220)
};

// 0-9 character binary map
const uint8_t numbers[10][8] = 
{
  {0b00000000, 0b01111111, 0b01000001, 0b01000001, 0b01000001, 0b01000001, 0b01111111, 0b00000000}, // 0
  {0b00000000, 0b00000000, 0b01000000, 0b01000001, 0b01111111, 0b01000000, 0b01000000, 0b00000000}, // 1
  {0b00000000, 0b01111001, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01001111, 0b00000000}, // 2
  {0b00000000, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01111111, 0b00000000}, // 3
  {0b00000000, 0b00001111, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b01111111, 0b00000000}, // 4
  {0b00000000, 0b01001111, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01111001, 0b00000000}, // 5
  {0b00000000, 0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01111001, 0b00000000}, // 6
  {0b00000000, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b01111111, 0b00000000}, // 7
  {0b00000000, 0b01111111, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01111111, 0b00000000}, // 8
  {0b00000000, 0b01001111, 0b01001001, 0b01001001, 0b01001001, 0b01001001, 0b01111111, 0b00000000}  // 9
};

// display 0-9 number at matrix position 0-7
void displayNum(int position, int num)
{
  position = (position % 8) * 8;

  for (int i = 0; i < 8; i++)
    matrix.setColumn(63 - (position + i), numbers[num][i]);
}

// display character on led at position
void displayChar(int position, char c, bool left=false) 
{
  uint8_t charBuffer[8];  // Buffer to hold the character columns
  uint8_t width = matrix.getChar(c, sizeof(charBuffer), charBuffer);  // Retrieve the bitmap for the character
  uint8_t start = left?62:30;

  for (int i = 0; i < 8; i++)
  {
    uint8_t p = start - (position + i);
    if (p >= 0 && p <= (start+1))
    {
      uint8_t b = (i < width)?charBuffer[i]:0;
      matrix.setColumn(p, b);
    }
  }
}

void displayString(String data, bool left=false)
{
  matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  uint8_t p = 0;
  for(int i=0; i<data.length(); i++)
  {
    displayChar(p,data[i], left);
    p += 8;
  }

  matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void displayScore(int s, int b = -1)
{
  matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  if (b >= 0)
  {
    // remaining balls
    displayNum(1, (b / 10) % 10);
    displayNum(2, b % 10);
  }

  // each digit at a time
  displayNum(4, s / 1000);
  displayNum(5, (s / 100) % 10);
  displayNum(6, (s / 10) % 10);
  displayNum(7, s % 10);

  matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void setup() 
{
  Serial.begin(115200);
  delay(500);

  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // configure sensor pins
  for(int i = 0; i < ULTRASONIC_SENSORS; i++)
    sensors[i].setup();

  // led matrix setup
  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, LED_BRIGHTNESS);
  matrix.clear();
  matrix.update();
}

void loop() 
{
  if (gameRunning)
  {
    // cycle through all sensors
    for (int i=0; i < ULTRASONIC_SENSORS; i++)
    {
      unsigned long currentMillis = millis();

      // debounce logic
      if (currentMillis > sensors[i].nextDetect)
      {
        unsigned long distance = sensors[i].getDistance();

        // write out to serial monitor sensor distance
        if (i == DEBUG_SENSOR)
          Serial.println(distance);

        if (distance <= sensors[i].detectRange)
        {
          // wait before we can rescan this sensor)
          for(int j=0;j<ULTRASONIC_SENSORS;j++)
            if (sensors[j].triggerPin == sensors[i].triggerPin)
              sensors[j].nextDetect = currentMillis + WAIT_BETWEEN_BALLS;

          gameTone.setTone(sensors[i].tone);
          
          score += sensors[i].points;  // add and display score
          balls--;  // one less ball

          if (score > highScore)
            highScore = score;

          displayScore(score, balls);

          // if it's our last ball then game is over
          if (balls <= 0)
          {
            gameRunning = false;
            cycle = 9;
          }
        }
      }
    }
  }
  else 
  {
    switch(cycle)
    {
      case 0:
        displayString("PING    ", true);
        cycleDelay = 500;
        cycle++;
        break;

      case 1:
        displayString("    PING", true);
        cycleDelay = 500;
        cycle++;
        break;

      case 2:
        displayString("PONG    ", true);
        cycleDelay = 500;
        cycle++;
        break;

      case 3:
        displayString("    PONG", true);
        cycleDelay = 500;
        cycle++;
        break;

      case 4:
        displayString("TOSS    ", true);
        cycleDelay = 500;
        cycle++;
        break;

      case 5:
        displayString("    TOSS", true);
        cycleDelay = 500;
        cycle++;
        break;

      case 6:
        displayString("LAST", true);
        displayScore(score);
        cycleDelay = 1500;
        cycle++;
        break;

      case 7:
        displayString("HIGH", true);
        displayScore(highScore);
        cycleDelay = 1500;
        cycle++;
        break;

      case 8:
        displayString("PUSH-GO-", true);
        cycleDelay = 2000;
        cycle = 0;
        break;

      case 9:
        displayString("GAMEOVER", true);
        cycleDelay = 2000;
        cycle = 6;
        break;
    }

    bool pressed = false;
    unsigned long expiry = millis() + cycleDelay;
    while(millis() < expiry)
    {
      if (digitalRead(BUTTON_PIN) == LOW)
      {
        pressed = true;
        break;
      }

      delay(10);
    }

    if (pressed)
    {
      gameTone.setTone(0);

      score = 0;
      balls = 9;
      gameRunning = true;

      displayString("-  -", true);
      displayScore(score, balls);
    }
  }

  gameTone.loop();
}
