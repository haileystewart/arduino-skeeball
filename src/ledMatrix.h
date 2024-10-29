#ifndef LEDMATRIX_H
#define LEDMATRIX_H

#include <MD_MAX72xx.h>

class LEDMatrix
{
public:
    LEDMatrix(uint8_t dataPin, uint8_t clkPin, uint8_t csPin);
    void begin();
    void clear();
    void update();
    void print(const String& data);
    void showScore(int score);
    void brightness(int level);

private:
    MD_MAX72XX matrix;
    void displayNum(int position, int num);
    void displayChar(int position, char c);
};

#endif
