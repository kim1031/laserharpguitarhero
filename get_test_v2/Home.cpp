#include "Home.h"
#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <string>

Home::Home()
{
    
}

void Home::display_entry(Adafruit_RA8875* tft) 
{
    //tft->fillScreen(RA8875_BLACK);
    tft->textMode();
    tft->textSetCursor(150, 200);
    tft->textTransparent(RA8875_WHITE);
    tft->textWrite("Laser Harp Guitar Hero");
    tft->textSetCursor(150, 250);
    tft->textWrite("Hit the leftmost laser to start");
}
