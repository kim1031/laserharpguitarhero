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
  //how to set text size wtf

  string song_string = "This_Love";
  Leaderboard board(song_string);
  board.get_leaderboard(request_buffer);
  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
  string response = response_buffer;
  board.parse_leaderboard(response);
  board.display_leaderboard(&tft);
}

//int scores[20];
//string users[20];
//char song_name[100];
//int array_size;
//
//void setup() {
//  Serial.begin(115200);
//  WiFi.begin(network, password);
//  uint8_t count = 0;;
//  while (WiFi.status() != WL_CONNECTED && count < 12) {
//    delay(500);
//    count++;
//  }
//  delay(2000);
//  if (WiFi.isConnected()) {
//    Serial.println("CONNECTED!");
//    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
//    delay(500);
//  } else {
//    ESP.restart();
//  }
//  if (!tft.begin(RA8875_800x480)) {
//    Serial.println("not found");
//    while (1);
//  }
//  tft.displayOn(true);
//  tft.GPIOX(true);
//  tft.PWM1config(true, RA8875_PWM_CLK_DIV1024);
//  tft.PWM1out(255);
//  tft.fillScreen(RA8875_BLACK);
//  tft.textMode();
//  tft.cursorBlink(32);
//  tft.textTransparent(RA8875_CYAN);
//  tft.textSetCursor(0, 0);
//  //how to set text size wtf
//
//  //have to get song_name from some other part of code
//  get_leaderboard(song_name);
//  string response = response_buffer;
//  parse_leaderboard(response);
//  display_leaderboard(song_name);
//}
//
//void loop() {
//  // put your main code here, to run repeatedly:
//  //don't need anything to loop rn
//}
//
//class Leaderboard{
//  
//}
//
//void display_leaderboard(char* song) {
//  char title[100] = "Leaderboards: ";
//  strcat(title, song);
//  tft.textSetCursor(0, 0);
//  tft.textWrite(title);
//  int place_count = 1;
//  char result_str[100];
//  char score_str[100];
//  int cursor_y;
//  for (int index = 0; index < array_size; index++) { //check that the length of the users list thing is right (boundary)
//    memset(result_str, 0, sizeof(result_str));
//    memset(score_str, 0, sizeof(score_str));
//    sprintf(result_str, "%d", place_count);
//    strcat(result_str, ". ");
//    strcat(result_str, users[index].c_str());
//    strcat(result_str, ": ");
//    sprintf(score_str, "%d", scores[index]);
//    strcat(result_str, score_str);
//    strcat(result_str, " points");
//    cursor_y = (1+index)*20;
//    tft.textSetCursor(10, cursor_y);
//    tft.textWrite(result_str);
//    place_count++;
//  }
//}
//
//void get_leaderboard(char* song) {
//  sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/leaderboards.py?song=");
//  strcat(request_buffer, song);
//  strcat(request_buffer, " HTTP/1.1\r\n");
//  strcat(request_buffer, "Host: 608dev.net\r\n");
//  strcat(request_buffer, "\r\n");
//  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
//  Serial.println(response_buffer);
//}
//
////have to pass in response_buffer but as a c++ string
//void parse_leaderboard(string str) {
//  int array_index = 0;
//  while (str.size() > 0) {
//    int score_end = str.find(",");
//    if (score_end != -1) {
//      scores[array_index] = atoi(str.substr(1, score_end).c_str());
//      str = str.substr(score_end+1);
//    }
//    else {
//      break;
//    }
//    int user_end = str.find("]");
//    if (user_end != -1){
//      users[array_index] = (str.substr(0, user_end));
//    }
//    else {
//      break;
//    }
//    str = str.substr(user_end+1);
//    array_index++;
//  }
//  array_size = array_index;
//  for (int ix = 0; ix < array_size; ix++) {
//    Serial.println(scores[ix]);
//  }
//  for (int indx = 0; indx < array_size; indx++) {
//    Serial.println(users[indx].c_str());
//  }
//}

//move these later
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
