/*
 * This is just driver code that we used for testing our Song Selection library! The actual code that controls song selection when playing the game can be found in the Game class :)
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
#include "SongSelection.h"

//defining pins for screen
#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21 

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);

//char network[] = "6s08";
//char password[] = "iesc6s08";
char network[] = "MIT";
char password[] = "";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 30000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];

string selected_song;
int scrolling_timer;
int scrolling_threshold = 750; //ms
bool choosing;
SongSelection song_selection;
string old_song;

void setup() {
  Serial.begin(115200);
  WiFi.begin(network);
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
  tft.textTransparent(RA8875_CYAN);
  tft.textSetCursor(0, 80);
  tft.textWrite("Use the first or second laser to scroll through songs. Break the third laser to select a song.");

  song_selection.get_song_selection(request_buffer);
  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  string response = response_buffer;
  song_selection.parse_song_selection(response);
  song_selection.display_song_selection(&tft); //just the 0th song at first.
  choosing = true;
}

void loop() {
  if (choosing) {
    int third_bins = analogRead(A3);
    float third_voltage = (third_bins / 4096.0) * 3.3;
    if (third_voltage >= 1.5) {
      Serial.println("laser 3 broken");
      update(3);
    }

    int first_bins = analogRead(A7);
    float first_voltage = (first_bins / 4096.0) * 3.3;
    if (first_voltage >= 1.5) {
      Serial.println("laser 1 broken");
      update(1);
    }

    int second_bins = analogRead(A6);
    float second_voltage = (second_bins / 4096.0) * 3.3;
    if (second_voltage >= 1.5) {
      Serial.println("laser 2 broken");
      update(2);
    }
  }
}

void update(int input) {
  switch(input){
    case 1: {  //scroll left
     if (millis() - scrolling_timer >= scrolling_threshold) {
      old_song = selected_song;
      song_selection.update_song_index(false);
      selected_song = song_selection.get_curr_song();
      if (old_song.compare(selected_song) != 0) {
        Serial.println("drawing 1");
        song_selection.display_song_selection(&tft);
      }     
      scrolling_timer = millis();
     }
     break;
    }
    case 2: { //scroll right
      if (millis() - scrolling_timer >= scrolling_threshold) {
        old_song = selected_song;
        song_selection.update_song_index(true);
        selected_song = song_selection.get_curr_song();
        if (old_song.compare(selected_song) != 0) {
          Serial.println("drawing 2");
          song_selection.display_song_selection(&tft);
        }
        scrolling_timer = millis();
      }
      break;
    }
   case 3: { //selected song
    selected_song = song_selection.get_curr_song();
    tft.fillRect(0, 0, 800, 300, RA8875_BLACK);
    tft.textTransparent(RA8875_CYAN);
    tft.textMode();
    tft.textSetCursor(10, 20);
    tft.textWrite("Nice choice!");
    char display_text[100] = "You selected ";
    strcat(display_text, selected_song.c_str());
    tft.textSetCursor(10, 40);
    tft.textWrite(display_text);
    tft.textSetCursor(10, 60);
    tft.textWrite("Break the first laser to continue.");
    choosing = false;
    break;
   }
  }
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
