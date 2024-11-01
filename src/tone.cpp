#include "tone.h"
#include <Arduino.h>

Tone::Tone() : toneToPlay(0), toneMillis(0) {}

void Tone::setTone(int tone)
{
    toneToPlay = tone;

    if (tone > 0)
        toneMillis = millis() - TONE_DURATION;
    else
       noTone(SPEAKER_PIN);;
}

void Tone::loop()
{
    unsigned long currentMillis = millis();

    if (toneMillis != 0 && currentMillis - toneMillis >= TONE_DURATION)
    {
        toneMillis = currentMillis;

        if (toneToPlay != 0)
        {
            tone(SPEAKER_PIN, toneToPlay, TONE_DURATION);
            toneToPlay = 0;
        }
        else
        {
            noTone(SPEAKER_PIN);
            toneMillis = 0;
        }       
    }
}
