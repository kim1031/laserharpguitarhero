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
  this->curr_index = 0; //stores current position in songs, artists, and durations arrays
  this->array_size = 0; //keep track of array size
  this->scrolling_timer = millis(); //initialize scrolling timer
}

//creates the request buffer for the get request that retrieves the song list from the server.
void SongSelection::get_song_selection(char* request_buffer) {
  sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_songlist.py");
  strcat(request_buffer, " HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net\r\n");
  strcat(request_buffer, "\r\n");
}

//parses the response from the get request
void SongSelection::parse_song_selection(std::string str) { //pass in response buffer but as a string
  int array_index = 0; //start at index zero
  while (str.size() > 0) { 
    int name_start = str.find("\'"); //find the location of the first aostrophe, which signifies the start of the name
    if (name_start != -1) { //if we found the apostrophe...
      str = str.substr(name_start+1);
      int name_end = str.find("\'"); //find the next apostrophe, which signifies the end of the name
      if (name_end != -1) {
        std::string song_name = str.substr(0, name_end); //create a substring of just the name and add it to the song name array
        songs[array_index] = song_name;
        str = str.substr(name_end+4); //remove song name from start of string
      }
      else {
        break;
      }
    }
    else {
      break;
    }
    int artist_end = str.find("\'"); //find the next apostrophe, which signifies the end of the artist's name
    if (artist_end != -1) {
      std::string artist_name = str.substr(0, artist_end); //get the substring of just the artist's name and add it to the artists array
      artists[array_index] = artist_name;
      str = str.substr(artist_end + 3); //remove artist name from start of string
    }
    else {
      break;
    }
    int duration_end = str.find(")"); //find the next closing parentheses, which signify the end of the duration
    if (duration_end != -1) {
      float dur = atof(str.substr(0, duration_end).c_str()); //get a substring of just the duration and convert it to a float before adding it to the durations array
      durations[array_index] = dur;
    }
    else {
      break;
    }
    array_index++; //increase array index
  }
  array_size = array_index; //at the end, the final array index is the size of the array
}

//updates the current index. takes in a boolean forward, which is true if we are scrolling forwards and false if we are scrolling backwards
void SongSelection::update_song_index(bool forward) {
  if (forward) {
    curr_index++;
  }
  else{
    curr_index--;
  }

  if (curr_index >= array_size) { //if the current index is greater than or equal to the array size, we set it to zero so that we scroll cyclically
    curr_index = 0;
  }
  else if (curr_index < 0) { //if the current index is less than zero, we set it to the maximum possible index so that we scroll cyclically
    curr_index = array_size - 1;
  }
}

void SongSelection::display_song_selection(Adafruit_RA8875* tft) {
  //display song information to TFT screen
  tft->fillRect(0, 10, 400, 70, RA8875_BLACK); //draw black rectangle to cover previous information
  tft->textTransparent(RA8875_CYAN);
  tft->textSetCursor(0, 0);
  tft->textWrite("Song Selection"); //title
  tft->textMode();
  tft->textSetCursor(10, 20);
  tft->textWrite(songs[curr_index].c_str()); //display current song name
  tft->textSetCursor(10, 40);
  tft->textWrite(artists[curr_index].c_str()); //display current artist name
  tft->textSetCursor(10, 60);
  float curr_dur = durations[curr_index];
  char dur_str[30];
  sprintf(dur_str, "%f", curr_dur);
  strcat(dur_str, " seconds");
  tft->textWrite(dur_str); //display current duration as a string
}

std::string SongSelection::get_curr_song() {
  return songs[curr_index]; //get current song name
}

//updates the screen based on which laser is selected
//input is a laser number (one being leftmost), tft is a pointer to the tft object
void SongSelection::update_screen(int input, Adafruit_RA8875* tft) {
  switch(input){
    case 1: {  //scroll left
     if (millis() - scrolling_timer >= scrolling_threshold) { //only scroll if enough time has elapsed
      old_song = selected_song; //keep track of old song
      update_song_index(false); //scroll backwards
      selected_song = get_curr_song();
      if (old_song.compare(selected_song) != 0) { //only draw if old song is different from new song
        display_song_selection(tft);
      }     
      scrolling_timer = millis(); //reset scrolling timer
     }
     break;
    }
    case 2: { //scroll right
      if (millis() - scrolling_timer >= scrolling_threshold) { //only scroll if enough time has elapsed
        old_song = selected_song;
        update_song_index(true);
        selected_song = get_curr_song();
        if (old_song.compare(selected_song) != 0) { //only draw if old song is different from current song
          display_song_selection(tft);
        }
        scrolling_timer = millis(); //reset scrolling timer
      }
      break;
    }
   case 3: { //user selected song. display choice to screen
    selected_song = get_curr_song();
    tft->fillRect(0, 0, 800, 400, RA8875_BLACK);
    tft->textTransparent(RA8875_CYAN);
    tft->textMode();
    tft->textSetCursor(10, 20);
    tft->textWrite("Nice choice!");
    char display_text[100] = "You selected ";
    strcat(display_text, selected_song.c_str());
    tft->textSetCursor(10, 40);
    tft->textWrite(display_text);
    break;
   }
  }
}

int SongSelection::get_song_num() {
  return curr_index + 1; 
}
