 #include "RectNote.h"
#include "LaserString.h"
#include "Game.h"
#include "Home.h"

#include <SPI.h>
#include <string>
#include <WiFi.h>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21

using namespace std;

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

char network[] = "MIT";
//char network[] = "6s08";
//char password[] = "iesc6s08";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 30000;
char request_buffer[IN_BUFFER_SIZE];
char old_request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];
 
int timer = 0;
bool not_set = true;
int loop_timer;
int elapsed = 0;

Game game(&tft, &myDFPlayer);

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

  mySoftwareSerial.begin(9600, SERIAL_8N1, 32, 33);  // speed, type, RX, TX
  Serial.println(F("DFRobot DFPlayer Mini Demo"));
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  delay(1000);
  if (!myDFPlayer.begin(mySoftwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(myDFPlayer.readType(), HEX);
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  myDFPlayer.setTimeOut(500); //Set serial communictaion time out 500ms
  myDFPlayer.volume(5);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  
  // score = 0;
  // a_hand = false;
  // s_hand = false;
  // d_hand = false;
  // f_hand = false;

  // a_inc = true;
  // s_inc = true;
  // d_inc = true;
  // f_inc = true;
  
  // pinMode(a_LED_pin, OUTPUT);
  // pinMode(s_LED_pin, OUTPUT);
  // pinMode(d_LED_pin, OUTPUT);
  // pinMode(f_LED_pin, OUTPUT);

  // digitalWrite(a_LED_pin, 0);
  // digitalWrite(s_LED_pin, 0);
  // digitalWrite(d_LED_pin, 0);
  // digitalWrite(f_LED_pin, 0);

  //tft.fillRect(0, 380, 800, 100, RA8875_WHITE);

  // getSong();
  // string response(response_buffer);
  // string_parser(response);
  // Serial.println(response.c_str());
  // transfer_notes();
  //Game game(&tft, &myDFPlayer);

  //game.setUp(&tft, &myDFPlayer)
  game.setUpLED();

  //myDFPlayer.play(5);  //Play the first mp3
  //timer = millis();
}

void loop() {
  loop_timer = millis();
  //Serial.print("Start: ");
  //Serial.println(millis());
  
  if (game.getState() == 4)
  {
    if (not_set)
    {
      timer = millis();
      not_set = false;
    }
    elapsed = millis() - timer;
  }
  //int elapsed = millis() - timer;
  Serial.println("hi");
  Serial.println(game.getState());
  Serial.println(game.getAStart());
  Serial.print("num: ");
  Serial.println(game.getNumARects());
  //Serial.println(response_buffer);
  Serial.println(elapsed);
  game.gamePlay(elapsed, request_buffer, response_buffer);

  if (strcmp(request_buffer, old_request_buffer) != 0)
  {
    do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    strcpy(old_request_buffer, request_buffer);
  }

  // if (a_rects[a_start].toPress()) {
  //   digitalWrite(a_LED_pin, 1);
  //   a_hand_in_timer = millis();
  // }
  // if (s_rects[s_start].toPress()) {
  //   digitalWrite(s_LED_pin, 1);
  //   s_hand_in_timer = millis();
  // }
  // if (d_rects[d_start].toPress()) {
  //   digitalWrite(d_LED_pin, 1);
  //   d_hand_in_timer = millis();
  // }
  // if (f_rects[f_start].toPress()) {
  //   digitalWrite(f_LED_pin, 1);
  //   f_hand_in_timer = millis();
  // }

  // if (a_rects[a_start].passed())
  // {
  //   digitalWrite(a_LED_pin, 0);
  //   a_start++;
  //   a_inc = false;
  //   //a_hand_out_timer = millis();
  // }
  // if (s_rects[s_start].passed())
  // {
  //   digitalWrite(s_LED_pin, 0);
  //   s_start++;
  //   s_inc = false;
  //   //s_hand_out_timer = millis();
  // }
  // if (d_rects[d_start].passed())
  // {
  //   digitalWrite(d_LED_pin, 0);
  //   d_start++;
  //   d_inc = false;
  //   //d_hand_out_timer = millis();
  // }
  // if (f_rects[f_start].passed())
  // {
  //   digitalWrite(f_LED_pin, 0);
  //   f_start++;
  //   f_inc = false;
  //   //f_hand_out_timer = millis();
  // }
  // update_all_hands();
  // Serial.print("Score: ");
  // Serial.println(score);

  while (millis() - loop_timer <= 20);
}

//void update_all_hands() {
//  int a_bins = analogRead(A0);
//  float a_voltage = (a_bins/4096.0)*3.3;
//  if (a_voltage >= 0.9 && (!a_hand)) {
//    actual_a_in = millis();
//    a_hand = true;
//  } else {
//    a_hand = false;
//  } 
//
//  if (((fabs(actual_a_in - a_hand_in_timer) <= 100) && a_hand) && !a_inc) {
//      int time_diff = (fabs(actual_a_in - a_hand_in_timer));
//      if (time_diff <= 10){
//        Serial.println("Perfect");
//        score += 5;
//      }
//      else if (time_diff <= 25){
//        Serial.println("Great");
//        score += 3;
//      }
//      else if (time_diff <= 50){
//        Serial.println("Good");
//        score += 2;
//      }
//      else if (time_diff <= 100){
//        Serial.println("Okay");
//        score += 1;
//      }
//      score += 1;
//      a_inc = true;
//  }
//  int s_bins = analogRead(A11);
//  float s_voltage = (s_bins / 4096.0) * 3.3;
//  if (s_voltage >= 0.9 && !s_hand) {
//    actual_s_in = millis();
//    s_hand = true;
//  } else 
//    s_hand = false;
//  if (((fabs(actual_s_in - s_hand_in_timer) <= 100) && s_hand) && !s_inc) {
//      int time_diff = (fabs(actual_s_in - s_hand_in_timer));
//      if (time_diff <= 10){
//        Serial.println("Perfect");
//        score += 5;
//      }
//      else if (time_diff <= 25){
//        Serial.println("Great");
//        score += 3;
//      }
//      else if (time_diff <= 50){
//        Serial.println("Good");
//        score += 2;
//      }
//      else if (time_diff <= 100){
//        Serial.println("Okay");
//        score += 1;
//      }
//      score += 1;
//      s_inc = true;
//  }
//  int d_bins = analogRead(A6);
//  float d_voltage = (d_bins / 4096.0) * 3.3;
//  Serial.print("D: ");
//  Serial.println(d_voltage);
//  if (d_voltage >= 0.9 && !d_hand) {
//    actual_d_in = millis();
//    d_hand = true;
//  } else
//    d_hand = false;
//  if (((fabs(actual_d_in - d_hand_in_timer) <= 100) && d_hand) && !d_inc) {
//      int time_diff = (fabs(actual_d_in - d_hand_in_timer));
//      if (time_diff <= 10){
//        Serial.println("Perfect");
//        score += 5;
//      }
//      else if (time_diff <= 25){
//        Serial.println("Great");
//        score += 3;
//      }
//      else if (time_diff <= 50){
//        Serial.println("Good");
//        score += 2;
//      }
//      else if (time_diff <= 100){
//        Serial.println("Okay");
//        score += 1;
//      }
//      score += 1;
//      d_inc = true;
//  }
//  int f_bins = analogRead(A7);
//  float f_voltage = (f_bins / 4096.0) * 3.3;
//  Serial.print("F: ");
//  Serial.println(f_voltage);
//  if (f_voltage >= 0.9 && !f_hand) {
//    actual_f_in = millis();
//    f_hand = true;
//  } else
//    f_hand = false;
//  if (((fabs(actual_f_in - f_hand_in_timer) <= 100) && f_hand) && !f_inc) {
//      int time_diff = (fabs(actual_f_in - f_hand_in_timer));
//      if (time_diff <= 10){
//        Serial.println("Perfect");
//        score += 5;
//      }
//      else if (time_diff <= 25){
//        Serial.println("Great");
//        score += 3;
//      }
//      else if (time_diff <= 50){
//        Serial.println("Good");
//        score += 2;
//      }
//      else if (time_diff <= 100){
//        Serial.println("Okay");
//        score += 1;
//      }
//      score += 1;
//      f_inc = true;
//  }
//}

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
