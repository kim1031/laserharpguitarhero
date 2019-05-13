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

#include <Arduino.h>
#include <DFRobotDFPlayerMini.h>

#define RA8875_INT 16
#define RA8875_CS  15
#define RA8875_RST 21

#define a_LED_pin 12
#define s_LED_pin 13
#define d_LED_pin 27
#define f_LED_pin 26

Adafruit_RA8875 tft = Adafruit_RA8875(RA8875_CS, RA8875_RST);
HardwareSerial mySoftwareSerial(2);
DFRobotDFPlayerMini myDFPlayer;

//char network[] = "MIT";
char network[] = "6s08";
char password[] = "iesc6s08";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 30000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];
 
int timer;
int loop_timer;


bool a_hand;
bool s_hand;
bool d_hand;
bool f_hand;

char song_list[500];
int song_indices[30]; //scores indices of song titles
int num_songs = 0;
char chosen[100];



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
  
  a_hand = false;
  s_hand = false;
  d_hand = false;
  f_hand = false;
  
  pinMode(a_LED_pin, OUTPUT);
  pinMode(s_LED_pin, OUTPUT);
  pinMode(d_LED_pin, OUTPUT);
  pinMode(f_LED_pin, OUTPUT);

  digitalWrite(a_LED_pin, 0);
  digitalWrite(s_LED_pin, 0);
  digitalWrite(d_LED_pin, 0);
  digitalWrite(f_LED_pin, 0);

  //tft.fillRect(0, 380, 800, 100, RA8875_WHITE);
  //tft.drawString("Score: ", 0, 350, RA8875_BLACK); //print out score

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
  myDFPlayer.volume(20);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  int delayms = 100;
  myDFPlayer.play(6);  //Play the first mp3
  timer = millis();
}

void loop() {
  get_songs();
  select_song();
}

void get_songs() {
  //need to put get_songlist on server
  char request[500];
  sprintf(request, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_songlist.py HTTP/1.1\r\n");
  strcat(request, "Host: 608dev.net\r\n");
  strcat(request, "\r\n");
  do_http_request("608dev.net", request, song_list, 500, RESPONSE_TIMEOUT, true);
  int i = 0;
  int j = 0;
  while (i <= strlen(song_list)) {
    if (&song_list[i] == "'") {
      song_indices[j] = i;
      j++;
    }
    i++;
  }
  num_songs = j%4;
}

void select_song() {
  string songs = song_list;

  //int len = song_indices[1]-1 - (song_indices[0]+1);
  //printf("%.*s\n", len, song_list + song_indices[0]+1);
  
  chosen = songs.substr(song_indices[0]+1,song_indices[1]-1);
  Serial.println(chosen);
  int counter = 0;
  while (f_hand == false) {
    int a_bins = analogRead(A0);
    float a_voltage = (a_bins/4096.0)*3.3;
    int s_bins = analogRead(A11);
    float s_voltage = (s_bins / 4096.0) * 3.3;
    int f_bins = analogRead(A7);
    float f_voltage = (f_bins / 4096.0) * 3.3;

    if (f_voltage >= 0.9) {
      f_hand = true;
    }
    
    if (a_voltage >= 0.9) {
      counter = (counter+1)%num_songs;
      chosen = songs.substr(song_indices[counter*2+1]+1,song_indices[counter*2+2]-1);
      Serial.println(chosen);
    }

    if (s_voltage >= 0.9) {
      counter = (counter-1)%num_songs;
      chosen = songs.substr(song_indices[counter*2+1]+1,song_indices[counter*2+2]-1);
      Serial.println(chosen);
    }

  }

  f_hand = false;
  
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
