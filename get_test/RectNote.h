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
        uint16_t color;

        void update_length();
        void update_y_coord();
        void draw_rect();
    public:
        void update();
};

#endif