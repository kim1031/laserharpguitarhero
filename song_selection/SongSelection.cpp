#include "SongSelection.h"

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <string>

SongSelection::SongSelection() {
  this->curr_index = 0;
  this->array_size = 0;
}

void SongSelection::get_song_selection(char* request_buffer) {
  sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_songlist.py");
  strcat(request_buffer, " HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net\r\n");
  strcat(request_buffer, "\r\n");
}

void SongSelection::parse_song_selection(std::string str) { //pass in response buffer but as a string
  int array_index = 0;
  while (str.size() > 0) {
    int name_start = str.find("\'");
    if (name_start != -1) {
      str = str.substr(name_start+1);
      int name_end = str.find("\'");
      if (name_end != -1) {
        std::string song_name = str.substr(0, name_end);
        songs[array_index] = song_name;
        str = str.substr(name_end+4);
      }
      else {
        break;
      }
    }
    else {
      break;
    }
    int artist_end = str.find("\'");
    if (artist_end != -1) {
      std::string artist_name = str.substr(0, artist_end);
      artists[array_index] = artist_name;
      str = str.substr(artist_end + 3);
    }
    else {
      break;
    }
    int duration_end = str.find(")");
    if (duration_end != -1) {
      float dur = atof(str.substr(0, duration_end).c_str());
      durations[array_index] = dur;
    }
    else {
      break;
    }
    array_index++;
  }
  array_size = array_index;
  for (int i = 0; i < array_size; i++) {
    Serial.println(songs[i].c_str());
  }
}

void SongSelection::update_song_index(bool forward) {
  if (forward) {
    curr_index++;
  }
  else{
    curr_index--;
  }

  if (curr_index >= array_size) {
    curr_index = 0;
  }
  else if (curr_index < 0) {
    curr_index = array_size - 1;
  }
}

void SongSelection::display_song_selection(Adafruit_RA8875* tft) {
  tft->fillScreen(RA8875_BLACK); //do we need this we'll see
  tft->textSetCursor(0, 0);
  tft->textWrite("Song Selection");
  tft->textSetCursor(10, 20);
  tft->textWrite(songs[curr_index].c_str());
  tft->textSetCursor(10, 40);
  tft->textWrite(artists[curr_index].c_str());
  tft->textSetCursor(10, 60);
  float curr_dur = durations[curr_index];
  char dur_str[30];
  sprintf(dur_str, "%f", curr_dur);
  tft->textWrite(dur_str);
}

std::string SongSelection::get_curr_song() {
  return songs[curr_index];
}
