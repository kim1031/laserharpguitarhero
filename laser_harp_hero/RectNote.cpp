/*
 * Class which controls the display of notes to be pressed by user in gameplay.
 */

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

RectNote::RectNote(float start_time, float end_time, int dur, int wid, int x, uint16_t color)
{
    this->start_time = start_time;  //start time and end time relative to timestamp in song
    this->end_time = end_time;
    //parameters of the note to control how it should be displayed on the screen to signal the user
    this->duration = dur;
    this->width = wid;
    this->length = 0;             //temporary, will increase until length = duration
    this->x = x;
    this->y = 0;
    this->press = false;
    this->out_of_bounds = false;
    this->color = color;
}

void RectNote::updateLength(int screen_bottom)
{   
    //grow the bar until it hits correct length (at top of display)
    if (y == 0 && length < duration)
        length += 3;

    //set length of the bar depending on how much can fit in screen (at bottom of display)
    if (y + duration >= screen_bottom) 
    {
        length = screen_bottom - y;
        press = true;                   //note should be broken
    }
}

void RectNote::updateYCoord(int screen_bottom)
{
    //create sense that note is moving down the screen
    if (length >= duration || (y > 0 && y <= screen_bottom))
        y += 3;
}

void RectNote::drawRect(Adafruit_RA8875* tft, bool old)
{
    //draw the rectangle ont he screen
    if (old)
        tft->fillRect(x, y, width, 3, RA8875_BLACK);
    else
        tft->fillRect(x, y, width, length, color);
}

void RectNote::update(int screen_bottom, Adafruit_RA8875* tft)
{
    //update all aspects of the rectangle including location on screen, size
    //draw the rectangle
    drawRect(tft, true);
    updateYCoord(screen_bottom);
    updateLength(screen_bottom);
    drawRect(tft, false);

    if (y > screen_bottom)
    {
      out_of_bounds = true;
      press = false;  //laser should stop being broken when the note ends (leaves the screen)
    }
}

bool RectNote::toPress()  //whether to press the laser for this note
{
  return press; 
}

bool RectNote::passed()  //whether this note has ended
{
  return out_of_bounds;
}

float RectNote::getStart()
{
  return start_time;
}

float RectNote::getEnd()
{
  return end_time;
}
