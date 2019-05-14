#ifndef UsernameGetter_h
#define UsernameGetter_h

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

class UsernameGetter
{
  private:
      const char alphabet[50] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";;
      char query_string[50];
      int char_index;
      unsigned long scrolling_timer;
      unsigned long choosing_timer;
      const int scrolling_threshold = 500;
      const int choosing_threshold = 750;
  public:
      UsernameGetter();
      void update_name(int input, char* output);
      void set_char_index(int num);
      void clear_query();
};

#endif
