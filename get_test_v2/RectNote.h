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
        RectNote(int dur, int wid, int x, uint16_t color);
        void update(int screen_bottom, Adafruit_RA8875* tft);
        bool to_press();
        bool passed();
        int getY();
    private:
        void update_length(int screen_bottom);
        void update_y_coord(int screen_bottom);
        void draw_rect(Adafruit_RA8875* tft, bool old);
};

#endif
