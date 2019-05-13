#include <SPI.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>
#include <string>

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21

using namespace std;

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);
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
  tft.textColor(RA8875_WHITE, RA8875_RED);
  tft.textSetCursor(100, 10);
  tft.textWrite("Hit the left/right laser to enter your name!");

  choosing = true;
}

class UsernameGetter 
{
  char alphabet[50] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char query_string[50] = {0};
  int char_index;
  unsigned long scrolling_timer;
  unsigned long choosing_timer;
  const int scrolling_threshold = 500;
  const int choosing_threshold = 750;
  public:
      UsernameGetter() 
      {
        char_index = 0;
        scrolling_timer = millis();
        choosing_timer = millis();
      }
      void update(int input, char* output)
      {
        switch(input) 
        {
            case 2:  //scroll left
            {
              if ( millis() - scrolling_timer >= scrolling_threshold) 
              {
                 char_index += 1;
                 if (char_index > 26)
                      char_index = 0;
                 scrolling_timer = millis();
              }
              sprintf(output, query_string);
              strncat(output, alphabet+char_index, 1);
              break;
            }
            case 1: //scroll right
            {
              if ( millis() - scrolling_timer >= scrolling_threshold) 
              {
                 char_index -= 1;
                 if (char_index < 0)
                      char_index = 26;
                 scrolling_timer = millis();
              }
              sprintf(output, query_string);
              strncat(output, alphabet+char_index, 1);
              break;
            }
            case 3: //add letter to username
            {
                if ( millis() - choosing_timer >= choosing_threshold) 
                {
                  strncat(query_string, alphabet+char_index, 1);
                  sprintf(output, query_string);
                  char_index = 0;
                  choosing_timer = millis();
                }
                break;
            }
            case 4: //this is username
            {
                choosing = false;
                tft.fillScreen(RA8875_BLACK);
                tft.textSetCursor(10,20);
                tft.textWrite("Nice pick,");
                tft.textSetCursor(10,40);
                tft.textWrite(response);
                break;
            }
        }
      }
};

UsernameGetter ug;

void loop() {
  //if 1st laser is broken, scroll left in alphabet
  //if 2nd laser is broken, scroll right in alphabet
  //if 3rd laser is broken, go to next character
  //if 4th laser is broken, this is final username
  Serial.print("Choosing: ");
  Serial.println(choosing);
  if (choosing) 
  {
    int four_bins = analogRead(A0);
    float four_voltage = (four_bins / 4096.0) * 3.3;
    Serial.println(four_voltage);
    if (four_voltage >= 0.9) {
      Serial.println("4th laser broken");
      ug.update(4, response);
    }
    int third_bins = analogRead(A3);
    float third_voltage = (third_bins / 4096.0) * 3.3;
    if (third_voltage >= 0.9) {
      Serial.println("3rd laser broken");
      ug.update(3, response);
    }
    int fir_bins = analogRead(A7);
    float fir_voltage = (fir_bins / 4096.0) * 3.3;
    if (fir_voltage >= 0.9) {
      Serial.println("1st laser broken");
      ug.update(1, response);
    }
    int sec_bins = analogRead(A6);
    float sec_voltage = (sec_bins / 4096.0) * 3.3;
    if (sec_voltage >= 0.9) {
      Serial.println("2nd laser broken");
      ug.update(2, response);
    }
    
    if (strcmp(response, old_response) != 0) {//only draw if changed!
      tft.fillScreen(RA8875_BLACK);
      tft.textSetCursor(0, 0);
      tft.textWrite(response);
    }
    memset(old_response, 0, sizeof(old_response));
    strcat(old_response, response);
    
    //tft.fillScreen(RA8875_BLACK);
    //tft.textWrite(response);
  }
}
