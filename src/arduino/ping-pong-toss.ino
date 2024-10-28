#include <MD_MAX72xx.h>
#include <SPI.h>

#define ULTRASONIC_SENSORS  6
#define WAIT_BETWEEN_BALLS  1000

// led matrix definitions
#define HARDWARE_TYPE       MD_MAX72XX::FC16_HW
#define LED_MODULES         4   // there are 4 8x8 modules
#define LED_BRIGHTNESS      1   // brightness level is 0..15
#define CS_PIN              10
#define DATA_PIN            11
#define CLK_PIN             13

struct skeeballSensor
{
  int triggerPin;
  int echoPin;
  int points;
  int detectRange;
};

MD_MAX72XX matrix = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, LED_MODULES);

const int speakerPin  = 9;      // speaker pin
const int melody[] = {262, 294, 330, 349};  // Notes (C4 to C5)
const int bright[] = {15, 10, 6, 1};

int currentNote = 0;
unsigned long previousMillis = 0;
const long interval = 100;  // Interval between notes

int score         = 0;
int highScore     = 0;
int balls         = 9;
int gameOverCount = 0;

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

// list of ultrasonic sensors
const skeeballSensor skeeballSensors[ULTRASONIC_SENSORS] =
{
  { A5, 2, 100, 300 },
  { A4, 3,  25, 290 },

  { A5, 2, 100, 300 },
  { A5, 2, 100, 300 },
  { A5, 2, 100, 300 },
  { A5, 2, 100, 300 }

//  { A3, 4, 300, 300 },
//  { A2, 5, 400, 300 },
//  { A1, 6, 500, 300 },
//  { A0, 7, 600, 300 }
};

unsigned long nextDetect[ULTRASONIC_SENSORS] = { 0,0,0,0,0,0 };

// display 0-9 number at position
void displayNum(int position, int num)
{
  position *= 8;

  for (int i = 0; i < 8; i++)
    matrix.setColumn(31-(position + i), numbers[num][i]);
}

// display character on led at position
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

// display string on led
void displayString(String data)
{
  matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

  uint8_t p = 0;
  for(int i=0; i<data.length(); i++)
  {
    displayChar(p,data[i]);
    p += 8;
  }

  matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

// display score
void displayScore(int score) 
{
    if (score < 0 || score > 9999) 
        return;

    matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);

    // each digit at a time
    displayNum(0, score / 1000);
    displayNum(1, (score / 100) % 10);
    displayNum(2, (score / 10) % 10);
    displayNum(3, score % 10);

    matrix.control(0, LED_MODULES-1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void startGame()
{
  noTone(speakerPin);

  score         = 0;
  balls         = 9;
  gameOverCount = 0;

  displayString("PING");
  delay(500);

  displayString("PONG");
  delay(500);

  displayString("TOSS");
  delay(500);

  displayScore(score);
}

void setup() 
{
  Serial.begin(115200);
  delay(500);

  // configure sensor pins
  for(int i = 0; i < ULTRASONIC_SENSORS; i++)
  {
    pinMode(skeeballSensors[i].triggerPin, OUTPUT);
    pinMode(skeeballSensors[i].echoPin, INPUT);
  }

  // led matrix setup
  matrix.begin();
  matrix.control(MD_MAX72XX::INTENSITY, LED_BRIGHTNESS);
  matrix.clear();
  matrix.update();

  startGame();
}

void loop() 
{
  // game over handling
  if (balls <= 0 && gameOverCount > 0)
  {
    if (score > highScore)
      highScore = score;

    noTone(speakerPin);

    displayString("GAME");
    delay(500);

    displayString("OVER");
    delay(500);

    displayScore(score);
    delay(1000);

    displayString("HIGH");
    delay(500);

    displayScore(highScore);
    delay(1000);

    gameOverCount--;

    // if we've shown enough then restart game
    if (gameOverCount <= 0)
      startGame();
  }
  else 
  {
    // cycle through all sensors
    for (int i=0; i < ULTRASONIC_SENSORS; i++)
    {
      // debounce logic
      if (millis() > nextDetect[i])
      {
        // trigger pulse
        digitalWrite(skeeballSensors[i].triggerPin, LOW);
        delayMicroseconds(2);

        digitalWrite(skeeballSensors[i].triggerPin, HIGH);
        delayMicroseconds(10);

        digitalWrite(skeeballSensors[i].triggerPin, LOW);

        unsigned long distance = pulseIn(skeeballSensors[i].echoPin, HIGH);

        if (distance <= skeeballSensors[i].detectRange)
        {
          previousMillis = millis() - interval;
          currentNote = 0;

          // add and display score
          score += skeeballSensors[i].points;
          displayScore(score);

          // one less ball
          balls--;

          // wait before we can rescan this sensor)
          nextDetect[i] = millis() + WAIT_BETWEEN_BALLS;
        }
      }
    }

    unsigned long currentMillis = millis();

    if (previousMillis != 0 && currentMillis - previousMillis >= interval) 
    {
      previousMillis = currentMillis;

      if (currentNote < sizeof(melody) / sizeof(melody[0])) 
      {
        matrix.control(MD_MAX72XX::INTENSITY, bright[currentNote]);

        tone(speakerPin, melody[currentNote], interval);
        currentNote++;
      }
      else 
      {
        noTone(speakerPin);
        previousMillis = 0;
        currentNote = 0;

          // if it's our last ball then game is over
          if (balls <= 0)
            gameOverCount = 2;

      }
    }
  }
}

