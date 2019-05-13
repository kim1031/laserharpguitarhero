#ifndef RectNote_h
#define RectNote_h

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

class RectNote
{
    private:
        float start_time;
        float end_time;
        int length;
        int width;
        int duration;
        int x;
        int y;
        bool press;
        bool out_of_bounds;
        uint16_t color;
    public:
        RectNote();
        RectNote(float start_time, float end_time, int dur, int wid, int x, uint16_t color);
        void update(int screen_bottom, Adafruit_RA8875* tft);
        bool toPress();
        bool passed();
        float getStart();
        float getEnd();
    private:
        void updateLength(int screen_bottom);
        void updateYCoord(int screen_bottom);
        void drawRect(Adafruit_RA8875* tft, bool old);
};

#endif
