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
  this->char_index = 1;
  this->scrolling_timer = millis();     //make sure scroll is at reasonable speed
  this->choosing_timer = millis();      //make sure choosing letter has buffer period
}

void UsernameGetter::update_name(int input, char* output)
{   // update what should be displayed to screen based off laser input
  switch (input)
  {
    case 2:  //scroll right 2nd laser broken
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
    case 1: //scroll left 1st laser broken
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
    case 3: //append letter to username 3rd laser broken
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
    //4th laser broken would lead to next state
  }
}

void UsernameGetter::set_char_index(int num)  //set starting index to parameter
{
  char_index = num;
}

void UsernameGetter::clear_query()            //clear query_string when restarting game
{
  memset(query_string, 0, sizeof(query_string));
}
