#ifndef TONE_H
#define TONE_H

#define SPEAKER_PIN 9
#define TONE_DURATION 200

class Tone
{
public:
    Tone();
    void setTone(int tone);
    void loop();

private:
    int toneToPlay;
    unsigned long toneMillis;
};

#endif
