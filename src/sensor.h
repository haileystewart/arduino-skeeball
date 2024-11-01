#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor 
{
public:
    uint8_t       triggerPin;
    uint8_t       echoPin;
    unsigned long detectRange;
    int           points;
    int           tone;
    unsigned long nextDetect;

    Sensor(uint8_t triggerPin, uint8_t echoPin, unsigned long detectRange, int points, int tone): 
      triggerPin(triggerPin), 
      echoPin(echoPin),
      detectRange(detectRange),
      points(points),
      tone(tone),
      nextDetect(0)
      {}

    void setup();
    unsigned long getDistance();
};

#endif // SENSOR_H
