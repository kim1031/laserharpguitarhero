#include <SPI.h>
#include <string>
#include <WiFi.h>
using namespace std;
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include "RectNote.h"

#define RA8875_INT 4
#define RA8875_CS  15
#define RA8875_RST 5

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);


char network[] = "MIT";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 30000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];

int timer;
int loop_timer;

char note_arr[500] = {0};
float note_time_arr[500] = {0};
float duration_arr[500] = {0};

float a_arr[200] = {0};
float s_arr[200] = {0};
float d_arr[200] = {0};
float f_arr[200] = {0};

RectNote a_rects[100];
RectNote *a_rect_index = a_rects;
RectNote s_rects[100];
RectNote *s_rect_index = s_rects;
RectNote d_rects[100];
RectNote *d_rect_index = d_rects;
RectNote f_rects[100];
RectNote *f_rect_index = f_rects;

bool a_note = false;
bool s_note = false;
bool d_note = false;
bool f_note = false;

int a_index = 0;
int s_index = 0;
int d_index = 0;
int f_index = 0;

void setup() {
  Serial.begin(115200);
  Serial.begin(115200);
  WiFi.begin(network);
  uint8_t count = 0;
  Serial.print("Attempting to connect to ");
  Serial.println(network);
  while (WiFi.status() != WL_CONNECTED && count < 12) {
    delay(500);
    Serial.print(".");
    count++;
  }
  delay(2000);
  if (WiFi.isConnected()) {
    Serial.println("CONNECTED!");
    Serial.println(WiFi.localIP().toString() + " (" + WiFi.macAddress() + ") (" + WiFi.SSID() + ")");
    delay(500);
  } else {
    Serial.println("Failed to Connect :/  Going to restart");
    Serial.println(WiFi.status());
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

  getSong();
  string response(response_buffer);
  string_parser(response);
  Serial.println(response.c_str());
  transfer_notes();
  timer = millis();
}

void loop() {
  loop_timer = millis();
  //Serial.print("Start: ");
  //Serial.println(millis());
  int elapsed = millis() - timer;
  if ( fabs( (a_arr[a_index] * 1000) - elapsed) <= 3000 ) {
    int dur = int((a_arr[a_index + 1] - a_arr[a_index]) * 1000);
    if (dur <= 500)
      dur = 20;
    else
      dur = int(dur / 25);
    *(a_rect_index++) = RectNote( dur, 150, 50, RA8875_GREEN );
    a_note = true;
    a_index += 2;
  }
  if ( fabs( (s_arr[s_index] * 1000) - elapsed) <= 3000 ) {
    int dur = int((s_arr[s_index + 1] - s_arr[s_index]) * 1000);
    if (dur <= 500)
      dur = 20;
    else
      dur = int(dur / 25);
    *(s_rect_index++) = RectNote( dur, 150, 250, RA8875_RED );
    s_note = true;
    s_index += 2;
  }
  if ( fabs( (d_arr[d_index] * 1000) - elapsed) <= 3000 ) {
    int dur = int((d_arr[d_index + 1] - d_arr[d_index]) * 1000);
    if (dur <= 500)
      dur = 20;
    else
      dur = int(dur / 25);
    *(d_rect_index++) = RectNote( dur, 150, 450, RA8875_YELLOW );
    d_note = true;
    d_index += 2;
  }
  if ( fabs( (f_arr[f_index] * 1000) - elapsed) <= 3000 ) {
    int dur = int((f_arr[f_index + 1] - f_arr[f_index]) * 1000);
    if (dur <= 500)
      dur = 20;
    else
      dur = int(dur / 25);
    *(f_rect_index++) = RectNote( dur, 150, 650, RA8875_BLUE );
    f_note = true;
    f_index += 2;
  }
  for (int i = 0; i < a_index; i++)
  {
    a_rects[i].update(480, &tft);
  }
  for (int i = 0; i < s_index; i++)
    s_rects[i].update(480, &tft);
  for (int i = 0; i < d_index; i++)
    d_rects[i].update(480, &tft);
  for (int i = 0; i < f_index; i++)
    f_rects[i].update(480, &tft);
  while (millis() - loop_timer <= 8);
  //Serial.print("End: ");
  //Serial.println(millis());
}

void string_parser(string str) {
  int array_index = 0;
  int total = 0;
  while (str.size() > 0)
  {
    int noteindex = str.find("'");
    if (noteindex != -1)
      note_arr[array_index] = str.at(noteindex + 1);
    else
      break;
    if (noteindex + 5 < str.size()) {
      str = str.substr(noteindex + 5, str.size() - (noteindex + 5));
      total += (noteindex + 5);
    }
    else
      break;
    int time_end = str.find(",");
    if (time_end != -1) {
      note_time_arr[array_index] = atof(str.substr(0, time_end).c_str());
    } else
      break;
    if (time_end + 2 < str.size()) {
      str = str.substr(time_end + 2, str.size() - (time_end + 2));
      total += (time_end + 2);
    }
    else
      break;
    int duration_end = str.find(")");
    if (duration_end != -1) {
      duration_arr[array_index] = atof(str.substr(0, duration_end).c_str());
    } else
      break;
    array_index++;
  }
}

void transfer_notes() {
  int aindex = 0;
  int sindex = 0;
  int dindex = 0;
  int findex = 0;
  for (int i = 0; i < strlen(note_arr); i++)
  {
    if (note_arr[i] == 'a') {
      //Serial.print(aindex);
      //Serial.println(" reahed a");
      a_arr[aindex] = note_time_arr[i];
      //Serial.println(a_arr[aindex]);
      aindex++;
      a_arr[aindex] = note_time_arr[i] + duration_arr[i];
      //Serial.println(a_arr[aindex]);
      aindex++;
    }
    if (note_arr[i] == 's') {
      s_arr[sindex++] = note_time_arr[i];
      s_arr[sindex++] = note_time_arr[i] + duration_arr[i];
    }
    if (note_arr[i] == 'd') {
      d_arr[dindex++] = note_time_arr[i];
      d_arr[dindex++] = note_time_arr[i] + duration_arr[i];
    }
    if (note_arr[i] == 'f') {
      f_arr[findex++] = note_time_arr[i];
      f_arr[findex++] = note_time_arr[i] + duration_arr[i];
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

void getSong() {
  sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_song.py?song=Twinkle_Twinkle HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net\r\n");
  strcat(request_buffer, "\r\n");
  do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}
