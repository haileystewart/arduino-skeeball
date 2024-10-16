#include <MD_MAX72xx.h>
#include <SPI.h>

// Define the number of devices we have in the chain
#define MAX_DEVICES 4

// Define pin numbers for the LED matrix
#define DATA_PIN    11  // MOSI
#define CLK_PIN     13  // SCK
#define CS_PIN      10  // SS

MD_MAX72XX matrix = MD_MAX72XX(MD_MAX72XX::FC16_HW, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

// Define a single trig pin for all sensors
const int triggerPin = 2;

// Define separate echo pins for each sensor
const int echoPin1 = 3;
const int echoPin2 = 4;
const int echoPin3 = 5;
const int echoPin4 = 6;

int score = 0;

void triggerSensors() 
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);

  digitalWrite(triggerPin, LOW);
}

int getDistance(int echoPin) 
{
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  return distance;
}

void displayScore(int score) 
{
  char scoreStr[5];
  snprintf(scoreStr, sizeof(scoreStr), "%4d", score);
  matrix.clear();

  for (int i = 0; i < 4; i++) 
    matrix.setChar(i, scoreStr[i]);

  matrix.update();
}

void setup() 
{
  Serial.begin(115200);

  // Initialize the single trigger pin
  pinMode(triggerPin, OUTPUT);

  // Initialize pins for each sensor's echo
  pinMode(echoPin1, INPUT);
  pinMode(echoPin2, INPUT);
  pinMode(echoPin3, INPUT);
  pinMode(echoPin4, INPUT);

  // Initialize the LED matrix
  matrix.begin();
  matrix.clear();
  matrix.update();
}

void loop() 
{
  // Trigger all sensors at once
  triggerSensors();

  // Read distances from each sensor
  int distance1 = getDistance(echoPin1);
  int distance2 = getDistance(echoPin2);
  int distance3 = getDistance(echoPin3);
  int distance4 = getDistance(echoPin4);

  // Score based on distances
  if (distance1 < 5) 
  {
    Serial.println("Score 10 points");
    score += 10;
    displayScore(score);
  }

  if (distance2 < 5) 
  {
    Serial.println("Score 20 points");
    score += 20;
    displayScore(score);
  }

  if (distance3 < 5) 
  {
    Serial.println("Score 30 points");
    score += 30;
    displayScore(score);
  }

  if (distance4 < 5) 
  {
    Serial.println("Score 40 points");
    score += 40;
    displayScore(score);
  }

  delay(100);  // Adjust as necessary
}
