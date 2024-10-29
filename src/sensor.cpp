#include "sensor.h"

void Sensor::setup()
{
  pinMode(triggerPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

unsigned long Sensor::getDistance()
{
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);
  return pulseIn(echoPin, HIGH);
}
