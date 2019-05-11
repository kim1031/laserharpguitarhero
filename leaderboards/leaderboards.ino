#include <string>
using namespace std;

#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include "RectNote.h"

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);

char network[] = "MIT";
//char network[] = "6s08";
//char password[] = "iesc6s08";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 30000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];

float scores[20];
char users[20];

char song_name[50];

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
  tft.setCursor(380, 0);
  tft.setTextSize(3);
  tft.setTextColor(RA8875_BLUE);
  tft.setTextWrap(true);

  //have to get song_name from some other part of code
  get_leaderboard(song_name);
  string response = response_buffer;
  parse_leaderboard(response);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  //don't need anything to loop rn
}

void display_leaderboard(char* song) {
  char title[100] = "Leaderboards: ";
  strcat(title, song);
  tft.setCursor(400, 30); // for title. don't know if this is a good spot or not
  tft.println(title);
  tft.println(); //blank line
  tft.setCursor(10, 50); //play with x and y values
  int place_count = 1;
  char result_str[10];
  char score_str[10];
  for (index = 0; index < strlen(users); index++) { //check that the length of the users list thing is right (boundary)
    sprintf(result_str, "%d", place_count);
    strcat(result_str, ". ");
    strcat(result_str, users[index]);
    strcat(result_str, ": ");
    sprintf(score_str, "%d", scores[index]);
    strcat(result_str, score_str);
    strcat(result_str, " points");
    tft.println(result_str);
    place_count++;
  }
}

void get_leaderboard(char* song) {
  sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/leaderboards.py?song=");
  strcat(request_buffer, song);
  strcat(request_buffer, " HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net\r\n");
  strcat(request_buffer, "\r\n");
  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}

//have to pass in response_buffer but as a c++ string
void parse_leaderboard(string str) { //rn just assuming this works --> should test later.
  int array_index = 0;
  while (str.size() > 0) {
    int score_end = str.find(",");
    if (score_end != -1) {
      scores[array_index] = atoi(str.substr(1, score_end).cstr());
      str = str.substr(score_end+1);
    }
    else {
      break;
    }
    int user_end = str.find("]");
    if (user_end != -1){
      users[array_index] = atoi(str.substr(0, user_end).cstr());
    }
    else {
      break;
    }
    str = str.substr(user_end+1);
    array_index++;
  }
}
