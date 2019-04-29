#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

#include "RectNote.h"

RectNote::RectNote(int dur, int len, int y, uint16_t color)
{
    this->duration = dur;
    this->width = 0;
    this->length = len; //temporary, will increase until length = duration
    this->x = 0;
    this->y = y;
    this->color = color;
}

void RectNote::update_length(int screen_bottom)
{   
    //grow the bar until it hits correct length
    if (x == 0 && width < duration)
        width++;

    //set length of the bar depending on how much can fit in screen
    if (x + duration > screen_bottom)
        width = screen_bottom - x;
}

void RectNote::update_x_coord(int screen_bottom)
{
    if (width == duration || (x > 0 && x != screen_bottom))
        x++;
}

void RectNote::draw_rect(Adafruit_RA8875* tft, bool old)
{
    if (old)
        tft->fillRect(x, y, width, 1, RA8875_BLACK);
    else
        tft->fillRect(x, y, width, length, color);
}

void RectNote::update(int screen_bottom, Adafruit_RA8875* tft)
{
    draw_rect(tft, true);
    update_y_coord(screen_bottom);
    update_length(screen_bottom);
    draw_rect(tft, false);
}