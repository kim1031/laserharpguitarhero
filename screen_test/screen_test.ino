/*
 * This is just code that we used to test whether or not we set up our screen correctly. We also used it when testing our RectNote library that is responsible for displaying the falling rectangles that represent notes
 */

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <Adafruit_RA8875.h>
#include <SPI.h>

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS,RA8875_RST);

void setup() {
  Serial.begin(115200); //for debugging if needed.
  if(!tft.begin(RA8875_800x480)) {
    Serial.println("not found");
    while(1);
  }

  //set up display
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  //fill screen with black and draw some circles to make sure screen is working!
  tft.fillScreen(RA8875_BLACK);
  tft.setRotation(0);
  tft.drawCircle(750, 80, 30, RA8875_MAGENTA);
  tft.drawCircle(750, 190, 30, RA8875_BLUE);
  tft.drawCircle(750, 300, 30, RA8875_YELLOW);
  tft.drawCircle(750, 420, 30, RA8875_RED);
  //timer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  //the_note.update(500, &tft); //testing RectNote functions
  //while (millis() - timer < 25);
  //timer = millis();
}
