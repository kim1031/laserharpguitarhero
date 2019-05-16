#ifndef StartScreen_h
#define StartScreen_h

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

class StartScreen
{
  private:
      
  public:
      StartScreen();
      void display_entry(Adafruit_RA8875* tft);
};

#endif
