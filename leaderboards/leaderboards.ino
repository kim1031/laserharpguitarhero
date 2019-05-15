/*
 * This is just driver code that we used to test our Leaderboards library when we were developing it! To see the actual leaderboards in action, check out the Game class :)
 */

#include <SPI.h>
#include <string>
#include <WiFi.h>
using namespace std;

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include "Leaderboard.h"

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21 

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);

char network[] = "6s08";
char password[] = "iesc6s08";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 30000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];

void setup() {
  Serial.begin(115200);
  WiFi.begin(network, password);
  uint8_t count = 0;;
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) {
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else {
    ESP.restart();
  }
  if (!tft.begin(RA8875_800x480)) {
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
  tft.textTransparent(RA8875_CYAN);
  tft.textSetCursor(0, 0);

  string song_string = "Paint_It_Black";
  Leaderboard board(song_string);
  board.get_leaderboard(request_buffer);
  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  string response = response_buffer;
  Serial.println(response_buffer);
  board.parse_leaderboard(response);
  board.display_leaderboard(&tft);
}

void loop()
{
  
}

uint8_t char_append(char* buff, char c, uint16_t buff_size) {
  int len = strlen(buff);
  if (len > buff_size) return false;
  buff[len] = c;
  buff[len + 1] = '\0';
  return true;
}

void do_http_request(char* host, char* request, char* response, uint16_t response_size, uint16_t response_timeout, uint8_t serial) {
  WiFiClient client; //instantiate a client object
  if (client.connect(host, 80)) { //try to connect to host on port 80
    if (serial) Serial.print(request);//Can do one-line if statements in C without curly braces
    client.print(request);
    memset(response, 0, response_size); //Null out (0 is the value of the null terminator '\0') entire buffer
    uint32_t count = millis();
    while (client.connected()) { //while we remain connected read out data coming back
      client.readBytesUntil('\n', response, response_size);
      if (serial) Serial.println(response);
      if (strcmp(response, "\r") == 0) { //found a blank line!
        break;
      }
      memset(response, 0, response_size);
      if (millis() - count > response_timeout) break;
    }
    memset(response, 0, response_size);
    count = millis();
    while (client.available()) { //read out remaining text (body of response)
      char_append(response, client.read(), OUT_BUFFER_SIZE);
    }
    if (serial) Serial.println(response);
    client.stop();
    if (serial) Serial.println("-----------");
  } else {
    if (serial) Serial.println("connection failed :/");
    if (serial) Serial.println("wait 0.5 sec...");
    client.stop();
  }
}
