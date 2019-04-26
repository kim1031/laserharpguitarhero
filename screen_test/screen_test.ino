#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <Adafruit_RA8875.h>
#include <SPI.h>

#define RA8875_INT 16//any pin
#define RA8875_CS  15//restriction for Teensy3 and CS
#define RA8875_RST 17//any pin

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS,RA8875_RST);

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  if(!tft.begin(RA8875_800x480)) {
    Serial.println("not found");
    while(1);
  }
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  tft.fillScreen(RA8875_BLACK);
  tft.setRotation(3);
  tft.drawCircle(750, 80, 30, RA8875_MAGENTA);
  tft.drawCircle(750, 190, 30, RA8875_BLUE);
  tft.drawCircle(750, 300, 30, RA8875_YELLOW);
  tft.drawCircle(750, 420, 30, RA8875_RED);

}

void loop() {
  // put your main code here, to run repeatedly:

}
