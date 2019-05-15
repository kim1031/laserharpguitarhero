#include <SPI.h>
#include <Arduino.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include "UsernameGetter.h"

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21

using namespace std;

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);
UsernameGetter ug;

char response[100];
char old_response[100];
bool choosing; 

void setup() {
  Serial.begin(115200);
  if (!tft.begin(RA8875_800x480)) 
  {
    Serial.println("not found");
    while (1);
  }
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);
  tft.fillScreen(RA8875_BLACK);
  tft.textMode();
  tft.cursorBlink(32);
  tft.textTransparent(RA8875_WHITE);
  tft.textSetCursor(100, 10);
  tft.textWrite("Hit the left/right laser to enter your name!");
  choosing = true;
}

void loop() {
  //if 1st laser is broken, scroll left in alphabet
  //if 2nd laser is broken, scroll right in alphabet
  //if 3rd laser is broken, go to next character
  //if 4th laser is broken, this is final username
  if (choosing) 
  {
    int four_bins = analogRead(A0);
    float four_voltage = (four_bins / 4096.0) * 3.3;
    if (four_voltage >= 1.5) {
      Serial.println("4th laser broken");
      choosing = false;
      tft.fillScreen(RA8875_BLACK);
      tft.textSetCursor(10,20);
      tft.textTransparent(RA8875_WHITE);
      tft.textWrite("Nice pick,");
      tft.textSetCursor(10,40);
      tft.textWrite(response);
    }
    int third_bins = analogRead(A3);
    float third_voltage = (third_bins / 4096.0) * 3.3;
    if (third_voltage >= 1.5) {
      Serial.println("3rd laser broken");
      ug.update_name(3, response);
    }
    int fir_bins = analogRead(A7);
    float fir_voltage = (fir_bins / 4096.0) * 3.3;
    if (fir_voltage >= 1.5) {
      Serial.println("1st laser broken");
      ug.update_name(1, response);
    }
    int sec_bins = analogRead(A6);
    float sec_voltage = (sec_bins / 4096.0) * 3.3;
    if (sec_voltage >= 1.5) {
      Serial.println("2nd laser broken");
      ug.update_name(2, response);
    }
    if (choosing) 
    {
      if (strcmp(response, old_response) != 0) //only draw if changed!
      {  
        tft.fillScreen(RA8875_BLACK);
        tft.textTransparent(RA8875_WHITE);
        tft.textSetCursor(0, 0);
        tft.textWrite(response);
      }
      memset(old_response, 0, sizeof(old_response));
      strcat(old_response, response); 
    }
  }
}
