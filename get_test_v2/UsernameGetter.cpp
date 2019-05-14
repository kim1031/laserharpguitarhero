#include "UsernameGetter.h"
#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

UsernameGetter::UsernameGetter()
{
  this->char_index = 0;
  this->scrolling_timer = millis();
  this->choosing_timer = millis();
}

void UsernameGetter::update_name(int input, char* output)
{
  switch (input)
  {
    case 2:  //scroll right
    {
        if ( millis() - scrolling_timer >= scrolling_threshold)
        {
          char_index += 1;
          if (char_index > 26)
            char_index = 0;
          scrolling_timer = millis();
        }
        sprintf(output, query_string);
        strncat(output, alphabet + char_index, 1);
        break;
    }
    case 1: //scroll left
    {
        if ( millis() - scrolling_timer >= scrolling_threshold)
        {
          char_index -= 1;
          if (char_index < 0)
            char_index = 26;
          scrolling_timer = millis();
        }
        sprintf(output, query_string);
        strncat(output, alphabet + char_index, 1);
        break;
    }
    case 3: //add letter to username
    {
        if ( millis() - choosing_timer >= choosing_threshold)
        {
          strncat(query_string, alphabet + char_index, 1);
          sprintf(output, query_string);
          char_index = 0;
          choosing_timer = millis();
        }
        break;
    }
  }
}
