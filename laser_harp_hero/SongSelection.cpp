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
  this->curr_index = 0; //keeping track of current index in arrays of songs, artists, and durations
  this->array_size = 0; //track size of array for when we need to loop
  this->scrolling_timer = millis(); //timer to prevent user from scrolling too quickly
}

void SongSelection::get_song_selection(char* request_buffer) { //create request buffer for http request
  sprintf(request_buffer, "GET http://608dev.net/sandbox/sc/jgonik/laserharpguitarhero/get_songlist.py");
  strcat(request_buffer, " HTTP/1.1\r\n");
  strcat(request_buffer, "Host: 608dev.net\r\n");
  strcat(request_buffer, "\r\n");
}

//parses the response of the get request into three different arrays: an array of the songs, an array of artists, and array of song durations.
//each song is represented by an index in the array. for instance, if you want the artist, song name, and duration for the sixth song, you would index into the sixth position in the corresponding arrays.
void SongSelection::parse_song_selection(std::string str) { //pass in response buffer but as a string.
  int array_index = 0; //keeps track of current index as we build up the arrays
  while (str.size() > 0) { //continue parsing until string is empty
    int name_start = str.find("\'"); //index of first apostrophe 
    if (name_start != -1) { //found apostrophe
      str = str.substr(name_start+1); //the name starts at the first character beyond the apostrophe -> makes a new substring of it.
      int name_end = str.find("\'"); //the name ends at the next apostrophe
      if (name_end != -1) { //found apostrophe
        std::string song_name = str.substr(0, name_end); //get substring of song name and add to songs array
        songs[array_index] = song_name;
        str = str.substr(name_end+4); //splice the string so that it starts at the artist name
      }
      else {
        break; //no apostrophe --> we're done
      }
    }
    else {
      break; // no apostrophe --> we're done
    }
    int artist_end = str.find("\'");
    if (artist_end != -1) { //found next apostrophe, signifying end of artist's name
      std::string artist_name = str.substr(0, artist_end); //get substring of artist's name and add to artists array
      artists[array_index] = artist_name;
      str = str.substr(artist_end + 3); //splice string so that it starts at the duration
    }
    else {
      break; //no apostrophe --> we're done
    }
    int duration_end = str.find(")"); //find next closing parentheses, which signifies end of duration
    if (duration_end != -1) { //found parentheses
      float dur = atof(str.substr(0, duration_end).c_str()); //convert substring of duration to floating point value and add it to durations array
      durations[array_index] = dur;
    }
    else {
      break; //no parentheses --> we're done
    }
    array_index++;
  }
  array_size = array_index; //the final array index is the size of all the arrays!
}

//updates the current index in the songs, artists, and durations arrays that we're at. used to scroll through different songs
//bool forward defines whether you're scrolling forwards or backwards (true if forwards, false if backwards)
void SongSelection::update_song_index(bool forward) {
  if (forward) {
    curr_index++;
  }
  else{
    curr_index--;
  }

  if (curr_index >= array_size) { //if larger than array size, scroll back to 0th position in array
    curr_index = 0;
  }
  else if (curr_index < 0) {
    curr_index = array_size - 1; //if less than zero, scroll to last value in the array
  }
}

//function that displays song selection on TFT screen
void SongSelection::display_song_selection(Adafruit_RA8875* tft) {
  tft->fillRect(0, 10, 300, 70, RA8875_BLACK); //draw a black rectangle that covers up the previous song's information (so there's no overlapping text)
  tft->textTransparent(RA8875_CYAN); //set text color to a pretty cyan
  tft->textSetCursor(0, 0); //set text cursor sets the starting position of where the text is going to be displayed
  tft->textWrite("Song Selection");
  tft->textMode(); 
  tft->textSetCursor(10, 20);
  tft->textWrite(songs[curr_index].c_str()); //display the song at the current index
  tft->textSetCursor(10, 40);
  tft->textWrite(artists[curr_index].c_str()); //display the artist at the current index
  tft->textSetCursor(10, 60);
  float curr_dur = durations[curr_index]; //get current duration and then make it a string and add " seconds" to the end
  char dur_str[30];
  sprintf(dur_str, "%f", curr_dur);
  strcat(dur_str, " seconds");
  tft->textWrite(dur_str); //display duration string
}

//getter function for current song
std::string SongSelection::get_curr_song() {
  return songs[curr_index];
}

//updates the tft display when scrolling through songs
void update_screen(int input, Adafruit_RA8875* tft) {
  switch(input){
    case 1: {  //scrolling left
     if (millis() - scrolling_timer >= scrolling_threshold) { //makes sure user doesn't scroll too fast
      old_song = selected_song;
      update_song_index(false); //scrolling backwards
      selected_song = get_curr_song();
      if (old_song.compare(selected_song) != 0) { //only reprint if song changed
        display_song_selection(tft);
      }     
      scrolling_timer = millis(); //reset timer
     }
     break;
    }
    case 2: { //scrolling right
      if (millis() - scrolling_timer >= scrolling_threshold) { //makes sure user doesn't scroll too fast
        old_song = selected_song;
        update_song_index(true); //scrolling forwards
        selected_song = get_curr_song();
        if (old_song.compare(selected_song) != 0) { //only reprint if song changed
          display_song_selection(tft);
        }
        scrolling_timer = millis();
      }
      break;
    }
   case 3: { //selected song
    selected_song = get_curr_song();
    tft->fillRect(0, 0, 800, 300, RA8875_BLACK); //draw black rectangle to cover up old display
    tft->textTransparent(RA8875_CYAN); //text color
    tft->textMode();
    tft->textSetCursor(10, 20);
    tft->textWrite("Nice choice!");
    char display_text[100] = "You selected ";
    strcat(display_text, selected_song.c_str()); //display the song that user selected
    tft->textSetCursor(10, 40);
    tft->textWrite(display_text);
    break;
   }
  }
}
