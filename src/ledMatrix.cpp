#include "ledMatrix.h"

#define LED_MODULES 8

LEDMatrix::LEDMatrix(uint8_t dataPin, uint8_t clkPin, uint8_t csPin) : matrix(MD_MAX72XX::FC16_HW, dataPin, clkPin, csPin, LED_MODULES) {}

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

void LEDMatrix::begin() 
{
    matrix.begin();
    clear();
    update();
}

void LEDMatrix::clear() 
{
    matrix.clear();
}

void LEDMatrix::update() 
{
    matrix.update();
}

void LEDMatrix::displayNum(int position, int num) 
{
    position *= 8;

    for (int i = 0; i < 8; i++) 
        matrix.setColumn(31 - (position + i), numbers[num][i]);
}

void LEDMatrix::displayChar(int position, char c) 
{
    uint8_t charBuffer[8];
    uint8_t width = matrix.getChar(c, sizeof(charBuffer), charBuffer);
    for (int i = 0; i < 8; i++) 
    {
        uint8_t p = 62 - (position + i);
        if (p >= 0 && p <= 63) 
        {
            uint8_t b = (i < width) ? charBuffer[i] : 0;
            matrix.setColumn(p, b);
        }
    }
}

void LEDMatrix::print(const String& data) 
{
    matrix.control(0, matrix.getDeviceCount() - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    uint8_t p = 0;
    for (int i = 0; i < data.length(); i++) 
    {
        displayChar(p, data[i]);
        p += 8;
    }
    matrix.control(0, matrix.getDeviceCount() - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void LEDMatrix::showScore(int score) 
{
    if (score < 0 || score > 9999) 
      return;

    matrix.control(0, matrix.getDeviceCount() - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::OFF);
    displayNum(0, score / 1000);
    displayNum(1, (score / 100) % 10);
    displayNum(2, (score / 10) % 10);
    displayNum(3, score % 10);
    matrix.control(0, matrix.getDeviceCount() - 1, MD_MAX72XX::UPDATE, MD_MAX72XX::ON);
}

void LEDMatrix::brightness(int level)
{
  matrix.control(MD_MAX72XX::INTENSITY, level);
}
