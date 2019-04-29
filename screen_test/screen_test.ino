#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <Adafruit_RA8875.h>
#include <SPI.h>

#include <WiFi.h> 

#include "RectNote.h"

#define RA8875_INT 4
#define RA8875_CS  15
#define RA8875_RST 5

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS,RA8875_RST);

char network[] = "MIT"; 
char password[] = ""; 
const int RESPONSE_TIMEOUT = 6000;       
const uint16_t IN_BUFFER_SIZE = 1000;     
const uint16_t OUT_BUFFER_SIZE = 1000;   
char request_buffer[IN_BUFFER_SIZE];     
char response_buffer[OUT_BUFFER_SIZE];  
RectNote the_note(100,40,20,RA8875_MAGENTA);
int timer;

void setup() {
  Serial.begin(115200);
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network, password); //attempt to connect to wifi
  uint8_t count = 0; //count used for Wifi check times
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) { //if we connected then print our IP, Mac, and SSID we're on
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else { //if we failed to connect just Try again.
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
    ESP.restart(); // restart the ESP (proper way)
  }
  if(!tft.begin(RA8875_800x480)) {
    Serial.println("not found");
    while(1);
  }
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  tft.fillScreen(RA8875_BLACK);
  tft.setRotation(0);
  //tft.drawCircle(750, 80, 30, RA8875_MAGENTA);
  //tft.drawCircle(750, 190, 30, RA8875_BLUE);
  //tft.drawCircle(750, 300, 30, RA8875_YELLOW);
  //tft.drawCircle(750, 420, 30, RA8875_RED);
  timer = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  the_note.update(500, &tft);
  while (millis() - timer < 25);
  timer = millis();
}
