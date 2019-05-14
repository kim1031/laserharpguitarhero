#include <SPI.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include "Home.h"

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21 

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);

void setup() {
  Serial.begin(115200);
  if (!tft.begin(RA8875_800x480)) {
    Serial.println("not found");
    while (1);
  }
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);
  Home home_screen(&tft);
}

void loop() {

}
