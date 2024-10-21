#define ULTRASONIC_SENSORS  2

struct skeeballSensor
{
  int           triggerPin;
  int           echoPin;
  int           points;
  int           lowDetectRange;
  int           highDetectRange;
  unsigned long nextDetect;
};

int score         = 0;
int balls         = 9;
int gameOverCount = 0;

// list of ultrasonic sensors
skeeballSensor skeeballSensors[ULTRASONIC_SENSORS] =
{
  { A5, 2, 10, 3, 30, 0 },
  { A4, 3, 20, 3, 30, 0 }
};

// display string on led
void displayString(String data)
{
  Serial.println(data);
}

// display score
void displayScore(int score) 
{
    if (score < 0 || score > 9999) 
        return;

    Serial.println("Score: " + String(score));
}

void startGame()
{
  score         = 0;
  balls         = 9;
  gameOverCount = 0;

  displayString("PING");
  delay(1000);

  displayString("PONG");
  delay(1000);

  displayString("TOSS");
  delay(1000);

  displayScore(score);
}

void setup() 
{
  Serial.begin(115200);
  delay(250);
  Serial.println("STARTUP");

  // configure sensor pins
  for(int i = 0; i < ULTRASONIC_SENSORS; i++)
  {
    pinMode(skeeballSensors[i].triggerPin, OUTPUT);
    pinMode(skeeballSensors[i].echoPin, INPUT);
  }

  startGame();
}

void loop() 
{
  // game over handling
  if (balls <= 0 && gameOverCount > 0)
  {
    displayScore(score);
    delay(1000);

    displayString("GAME");
    delay(1000);

    displayString("OVER");
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
      if (millis() > skeeballSensors[i].nextDetect)
      {
        // trigger pulse
        digitalWrite(skeeballSensors[i].triggerPin, LOW);
        delayMicroseconds(2);

        digitalWrite(skeeballSensors[i].triggerPin, HIGH);
        delayMicroseconds(10);

        digitalWrite(skeeballSensors[i].triggerPin, LOW);

        // calculate distance in cm
        int distance = (pulseIn(skeeballSensors[i].echoPin, HIGH) / 2) / 29.1;

        if (distance >= skeeballSensors[i].lowDetectRange && distance <= skeeballSensors[i].highDetectRange)
        {
          // add and display score
          score += skeeballSensors[i].points;
          displayScore(score);

          // one less ball
          balls--;

          // wait a second before we can count this sensor as points
          skeeballSensors[i].nextDetect = millis() + 1000;

          // if it's our last ball then game is over
          if (balls <= 0)
            gameOverCount = 5;
        }
      }
    }
  }
}
