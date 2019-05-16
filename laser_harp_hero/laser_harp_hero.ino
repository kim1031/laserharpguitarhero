#include "RectNote.h"
#include "LaserString.h"
#include "Leaderboard.h"
#include "Game.h"
#include "StartScreen.h"
#include "UsernameGetter.h"
#include "SongSelection.h"

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

//char network[] = "MIT";
char network[] = "6s08";
char password[] = "iesc6s08";
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

Game game(&tft, &myDFPlayer);  //initialize game object which will control gameplay

void setup() {
  Serial.begin(115200);
  //set up wifi to access server
  WiFi.begin(network,password);
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

  //set up display screen
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
  tft.textSetCursor(0, 0);

  //set up mp3 player
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
  myDFPlayer.volume(15);  //Set volume value (0~30).
  myDFPlayer.EQ(DFPLAYER_EQ_NORMAL);
  myDFPlayer.outputDevice(DFPLAYER_DEVICE_SD);
  
  //make sure LEDs are ready to be used
  game.setUpLED();
}

void loop() {
  loop_timer = millis();
  Serial.println(game.getState());
  if (game.getState() == 4) //only begin updating the elapsed time when in the Game Play State
  {
    if (not_set)
    {
      timer = millis();
      not_set = false;
    }
    elapsed = millis() - timer; //how much of the song's gameplay length has elapsed
  }
  if (game.getState() < 4) //be ready to calculate elapsed with respect to a new reference before entering Game Play State 4
    not_set = true;
  //loop through the game, updating states when necessary and interacting with user input
  game.gamePlay(elapsed, request_buffer, response_buffer);

  //perform an http request each time a new one comes in
  if (strcmp(request_buffer, old_request_buffer) != 0)
  {
    do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
    strcpy(old_request_buffer, request_buffer);
  }

   Serial.print("Score: ");
   Serial.println(game.getScore());

  while (millis() - loop_timer <= 20);
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
