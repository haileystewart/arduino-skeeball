#include <Arduino.h>
#include "ledMatrix.h"
#include "sensor.h"

// pin definition
#define BUTTON_PIN  8
#define SPEAKER_PIN 9
#define CS_PIN      10
#define DATA_PIN    11
#define CLK_PIN     13

LEDMatrix Display(DATA_PIN, CLK_PIN, CS_PIN);

#define ULTRASONIC_SENSORS 6

Sensor sensors[ULTRASONIC_SENSORS] =
{
  Sensor(A5, 2,  100, 300),
  Sensor(A4, 3,   25, 550),
  Sensor(A5, 2,  100, 300),
  Sensor(A5, 2,  100, 300),
  Sensor(A5, 2,  100, 300),
  Sensor(A5, 2,  100, 300)
//  { A3, 4, 300, 300 },
//  { A2, 5, 400, 300 },
//  { A1, 6, 500, 300 },
//  { A0, 7, 600, 300 }
};

#define DELAY_BETWEEN_BALLS  1000

const int melody[] = {262, 294, 330, 349};  // Notes (C4 to C5)
const int bright[] = {15, 10, 6, 1};

int currentNote = 0;
unsigned long previousMillis = 0;
const long interval = 100;  // Interval between notes

int score         = 0;
int highScore     = 0;
int balls         = 9;
int gameOverCount = 0;

void startGame()
{
  noTone(SPEAKER_PIN);

  score         = 0;
  balls         = 9;
  gameOverCount = 0;

  Display.print("PINGPONG");
  delay(500);

  Display.print("TOSS    ");
  delay(500);

  Display.print("    TOSS");
  delay(500);

  Display.showScore(score);
}

void gameOver()
{
  if (score > highScore)
    highScore = score;

  gameOver();
  noTone(SPEAKER_PIN);

  Display.print("GAME");
  delay(500);

  Display.print("OVER");
  delay(500);

  Display.showScore(score);
  delay(1000);

  Display.print("HIGH");
  delay(500);

  Display.showScore(highScore);
  delay(1000);

  gameOverCount--;

  // if we've shown enough then restart game
  if (gameOverCount <= 0)
    startGame();
}

void gameLoop()
{
  // cycle through all sensors
  for (int i=0; i < ULTRASONIC_SENSORS; i++)
  {
    // debounce logic
    if (millis() > sensors[i].nextDetect)
    {
      unsigned long distance = sensors[i].getDistance();

      if (i == 1)
        Serial.println(distance);

      if (distance <= sensors[i].detectRange)
      {
        previousMillis = millis() - interval;
        currentNote = 0;

        // add and display score
        score += sensors[i].points;
        Display.showScore(score);

        // one less ball
        balls--;

        // wait before we can rescan this sensor)
        sensors[i].nextDetect = millis() + DELAY_BETWEEN_BALLS;
      }
    }
  }
}

void soundLoop()
{
  unsigned long currentMillis = millis();

  if (previousMillis != 0 && currentMillis - previousMillis >= interval) 
  {
    previousMillis = currentMillis;

    if (currentNote < sizeof(melody) / sizeof(melody[0])) 
    {
      Display.brightness(bright[currentNote]);

      tone(SPEAKER_PIN, melody[currentNote], interval);
      currentNote++;
    }
    else 
    {
      noTone(SPEAKER_PIN);
      previousMillis = 0;
      currentNote = 0;

        // if it's our last ball then game is over
        if (balls <= 0)
          gameOverCount = 2;

    }
  }
}

void setup()
{
  Serial.begin(115200);
  delay(500);

  // configure sensor pins
  for(int i = 0; i < ULTRASONIC_SENSORS; i++)
    sensors[i].setup();

  Display.begin();

  startGame();
}

void loop() 
{
  if (balls > 0 || gameOverCount <= 0)
    gameLoop();
  else
    gameOver();

  soundLoop();
}
