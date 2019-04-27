#include <SPI.h>
#include <string>
#include <WiFi.h>
using namespace std;

char network[] = "MIT";
const int RESPONSE_TIMEOUT = 6000;
const uint16_t IN_BUFFER_SIZE = 1000;
const uint32_t OUT_BUFFER_SIZE = 50000;
char request_buffer[IN_BUFFER_SIZE];
char response_buffer[OUT_BUFFER_SIZE];

char note_arr[1000] = {0};
float note_time_arr[2000] = {0};
float duration_arr[2000] = {0};

void setup() {
  Serial.begin(115200);
  Serial.begin(115200); //for debugging if needed.
  WiFi.begin(network); //attempt to connect to wifi
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

  getSong();
  Serial.println(strlen(response_buffer));
  string response(response_buffer);
  string_parser(response);
  //Serial.println(response.c_str());
}

void loop() {
  // put your main code here, to run repeatedly:

}

void string_parser(string str) {
  int array_index = 0;
  int total = 0;
  while (str.size() > 0)
  {
    //Serial.println(str.c_str());
    int noteindex = str.find("'");
    Serial.print("Note Index: ");
    Serial.println(noteindex);
    if (noteindex != -1)
      note_arr[array_index] = str.at(noteindex+1);
    else
      break;
    if (noteindex+5 < str.size()) {
      Serial.println("getting substr");
      str = str.substr(noteindex+5, str.size()-(noteindex+5));
      total += (noteindex+5);
    }
    else
      break;
    int time_end = str.find(",");
    Serial.print("Time End: ");
    Serial.println(time_end);
    if (time_end != -1) {
      note_time_arr[array_index] = atof(str.substr(0, time_end).c_str());
    } else 
      break;
    if (time_end+2 < str.size()) {
      str = str.substr(time_end+2, str.size()-(time_end+2));
      total += (time_end+2);
    }
    else
      break;
    int duration_end = str.find(")");
    Serial.print("Duration End: ");
    Serial.println(duration_end);
    if (duration_end != -1) {
      duration_arr[array_index] = atof(str.substr(0, duration_end).c_str());
    } else
      break;
    array_index++;
  }
  Serial.println(note_arr);
  Serial.println(strlen(note_arr));
  Serial.println(total);
  /*for (int i = 0; i < sizeof(note_time_arr); i++) {
    Serial.print(note_time_arr[i]);
    Serial.print(" ");
  }
  for (int i = 0; i < sizeof(duration_arr); i++) {
    Serial.print(duration_arr[i]);
    Serial.print(" ");
  }*/
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
    sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_song.py?song=Barracuda HTTP/1.1\r\n");
    strcat(request_buffer,"Host: 608dev.net\r\n");
    strcat(request_buffer,"\r\n");
    do_http_request("608dev.net", request_buffer, response_buffer, OUT_BUFFER_SIZE, RESPONSE_TIMEOUT, true);
}
