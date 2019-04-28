#include "Arduino.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include "RectNote.h"

RectNote::RectNote(int dur, int wid, int x, uint16_t color)
{
    this->duration = dur;
    this->width = wid;
    this->length = 0; //temporary, will increase until length = duration
    this->x = x;
    this->y = 0;
    this->color = color;
}

void RectNote::update_length(int screen_bottom)
{   
    //grow the bar until it hits correct length
    if (y == 0 && length < duration)
        length++;

    //set length of the bar depending on how much can fit in screen
    if (y + duration > screen_bottom)
        len = screen_bottom - y;
}

void RectNote::update_y_coord(int screen_bottom)
{
    if (length == duration || (y > 0 && y != screen_bottom))
        y++;
}

void RectNote::draw_rect(Adafruit_RA8875* tft, bool old=False)
{
    if (old)
        tft.fillRect(x, y, width, length, RA8875_BLACK);
    else
        tft.fillRect(x, y, width, length, color);
}

void RectNote::update(int screen_bottom, Adafruit_RA8875* tft)
{
    draw_rect(tft, true);
    update_y_coord(screen_bottom);
    update_length(screen_bottom);
    draw_rect(tft, false);
}