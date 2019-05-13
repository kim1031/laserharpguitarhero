#include "RectNote.h"

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

RectNote::RectNote()
{ 
    this->start_time = 0;
    this->end_time = 0;
}

RectNote::RectNote(int start_time, int end_time, int dur, int wid, int x, uint16_t color)
{
    this->start_time = start_time;
    this->end_time = end_time;
    this->duration = dur;
    this->width = wid;
    this->length = 0; //temporary, will increase until length = duration
    this->x = x;
    this->y = 0;
    this->press = false;
    this->out_of_bounds = false;
    this->color = color;
}

void RectNote::updateLength(int screen_bottom)
{   
    //grow the bar until it hits correct length
    if (y == 0 && length < duration)
        length += 3;

    //set length of the bar depending on how much can fit in screen
    if (y + duration >= screen_bottom) 
    {
        length = screen_bottom - y;
        press = true;
    }
}

void RectNote::updateYCoord(int screen_bottom)
{
    if (length >= duration || (y > 0 && y <= screen_bottom))
        y += 3;
}

void RectNote::drawRect(Adafruit_RA8875* tft, bool old)
{
    if (old)
        tft->fillRect(x, y, width, 3, RA8875_BLACK);
    else
        tft->fillRect(x, y, width, length, color);
}

void RectNote::update(int screen_bottom, Adafruit_RA8875* tft)
{
    draw_rect(tft, true);
    update_y_coord(screen_bottom);
    update_length(screen_bottom);
    draw_rect(tft, false);

    if (y > screen_bottom)
    {
      out_of_bounds = true;
      press = false;
    }
}

bool RectNote::toPress()
{
  return press; 
}

bool RectNote::passed()
{
  return out_of_bounds;
}

int RectNote::getY()
{
  return y;
}

float RectNote::getStart()
{
  return start_time;
}

float RectNote::getEnd()
{
  return end_time;
}
