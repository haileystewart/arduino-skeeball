#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>

class Sensor 
{
public:
    uint8_t triggerPin;
    uint8_t echoPin;
    int     points;
    int     detectRange;
    long    nextDetect;

    Sensor(uint8_t trigger, uint8_t echo, int pts, int detect): 
      triggerPin(trigger), 
      echoPin(echo),
      points(pts),
      detectRange(detect),
      nextDetect(0){}

    void setup();
    unsigned long getDistance();
};

#endif // SENSOR_H
