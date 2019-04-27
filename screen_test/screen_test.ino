#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>

#include <Adafruit_RA8875.h>
#include <SPI.h>

#include <WiFi.h> 

#define RA8875_INT 16//any pin
#define RA8875_CS  15//restriction for Teensy3 and CS
#define RA8875_RST 17//any pin

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS,RA8875_RST);

char network[] = "MIT"; 
char password[] = ""; 
const int RESPONSE_TIMEOUT = 6000;       
const uint16_t IN_BUFFER_SIZE = 1000;     
const uint16_t OUT_BUFFER_SIZE = 1000;   
char request_buffer[IN_BUFFER_SIZE];     
char response_buffer[OUT_BUFFER_SIZE];  

void setup() {
  Serial.begin(115200);
  WiFi.begin(network,password);
  if(!tft.begin(RA8875_800x480)) {
    Serial.println("not found");
    while(1);
  }
  tft.displayOn(true);
  tft.GPIOX(true);
  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
  tft.PWM1out(255);

  getSong();
  Serial.println(request_buffer);
}

void loop() {
  // put your main code here, to run repeatedly:

}

void getSong() {
    sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_song.py?song=Barracuda HTTP/1.1\r\n");
    strcat(request_buffer,"Host: 608dev.net\r\n");
    strcat(request_buffer,"\r\n");
    do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}
