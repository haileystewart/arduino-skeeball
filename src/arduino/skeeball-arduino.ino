#include <MD_MAX72xx.h>
#include <SPI.h>

#define HARDWARE_TYPE       MD_MAX72XX::FC16_HW
#define LED_MODULES         4
#define ULTRASONIC_SENSORS  2
#define DATA_PIN            11
#define CS_PIN              12
#define CLK_PIN             13

struct skeeballSensor
{
  int           triggerPin;
  int           echoPin;
  int           points;
  int           lowDetectRange;
  int           highDetectRange;
  unsigned long nextDetect;
};

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, LED_MODULES);

int score         = 0;
int balls         = 9;
int gameOverCount = 0;

const uint8_t numbers[10][8] = 
{
    {0b00000000, 0b11111111, 0b10000001, 0b10000001, 0b10000001, 0b10000001, 0b11111111, 0b00000000}, // 0
    {0b00000000, 0b00000000, 0b10000000, 0b10000001, 0b11111111, 0b10000000, 0b10000000, 0b00000000}, // 1
    {0b00000000, 0b11111001, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b10001111, 0b00000000}, // 2
    {0b00000000, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b11111111, 0b00000000}, // 3
    {0b00000000, 0b00001111, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b11111111, 0b00000000}, // 4
    {0b00000000, 0b10001111, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b11111001, 0b00000000}, // 5
    {0b00000000, 0b11111111, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b11111001, 0b00000000}, // 6
    {0b00000000, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b00000001, 0b11111111, 0b00000000}, // 7
    {0b00000000, 0b11111111, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b11111111, 0b00000000}, // 8
    {0b00000000, 0b10001111, 0b10001001, 0b10001001, 0b10001001, 0b10001001, 0b11111111, 0b00000000}  // 9
};

skeeballSensor skeeballSensors[ULTRASONIC_SENSORS] =
{
  { A0, 2, 10, 3, 30, 0 },
  { A1, 3, 20, 3, 30, 0 }
};

void displayNum(int position, int num)
{
  position *= 8;

  for (int i = 0; i < 8; i++)
    matrix.setColumn(31-(position + i), numbers[num][i]);
}

void displayChar(int position, char c) 
{
  uint8_t charBuffer[8];  // Buffer to hold the character columns
  uint8_t width = matrix.getChar(c, sizeof(charBuffer), charBuffer);  // Retrieve the bitmap for the character

  for (int i = 0; i < 8; i++)
  {
    uint8_t p = 30 - (position + i);
    if (p >= 0 && p <= 31)
    {
        uint8_t b = (i < width)?charBuffer[i]:0;
        matrix.setColumn(p, b);
    }
  }
}

void displayString(String data)
{
  uint8_t p = 0;
  for(int i=0; i<data.length(); i++)
  {
    displayChar(p,data[i]);
    p += 8;
  }
}

void displayScore(int score) 
{
    if (score < 0 || score > 9999) 
        return;

    displayNum(0, score / 1000);
    displayNum(1, (score / 100) % 10);
    displayNum(2, (score / 10) % 10);
    displayNum(3, score % 10);
}

void startGame()
{
  score         = 0;
  balls         = 9;
  gameOverCount = 0;

  displayString("SKEE");
  delay(1000);

  displayString("BALL");
  delay(1000);

  displayScore(score);
}

void setup() 
{
  for(int i = 0; i < ULTRASONIC_SENSORS; i++)
  {
    pinMode(skeeballSensors[i].triggerPin, OUTPUT);
    pinMode(skeeballSensors[i].echoPin, INPUT);
  }

  matrix.begin();  // Initialize the matrix
  matrix.control(MD_MAX72XX::INTENSITY, 1);  // Set brightness level (0-15)
  matrix.clear();  // Clear the display
  matrix.update();

  startGame();
}

void loop() 
{
  if (balls <= 0 && gameOverCount > 0)
  {
    displayScore(score);
    delay(1000);

    displayString("GAME");
    delay(1000);

    displayString("OVER");
    delay(1000);

    gameOverCount--;

    if (gameOverCount <= 0)
      startGame();
  }
  else 
  {
    for (int i=0; i < ULTRASONIC_SENSORS; i++)
    {
      if (millis() > skeeballSensors[i].nextDetect)
      {
        // Trigger pulse
        digitalWrite(skeeballSensors[i].triggerPin, LOW);
        delayMicroseconds(2);

        digitalWrite(skeeballSensors[i].triggerPin, HIGH);
        delayMicroseconds(10);

        digitalWrite(skeeballSensors[i].triggerPin, LOW);

        // Measure echo time
        int duration = pulseIn(skeeballSensors[i].echoPin, HIGH);
        
        // Calculate distance in cm
        int distance = (duration / 2) / 29.1;

        if (distance >= skeeballSensors[i].lowDetectRange && distance <= skeeballSensors[i].highDetectRange)
        {
          score += skeeballSensors[i].points;
          displayScore(score);
          balls--;
          skeeballSensors[i].nextDetect = millis() + 1000;

          if (balls <= 0)
            gameOverCount = 5;
        }
      }
    }
  }
}
