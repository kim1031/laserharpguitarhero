#ifndef SongSelection_h
#define SongSelection_h

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <string>

class SongSelection {
  private: 
    std::string songs[30];
    std::string artists[30];
    float durations[30];
    int array_size;
    int curr_index;
    int scrolling_timer;
    int scrolling_threshold = 750; //ms
    std::string old_song;
    std::string selected_song;
    
    

  public:
    SongSelection();
    void parse_song_selection(std::string str);
    void get_song_selection(char* request_buffer);
    void display_song_selection(Adafruit_RA8875* tft);
    void update_song_index(bool forward);
    std::string get_curr_song();
    void update_screen(int input, Adafruit_RA8875* tft);
};

#endif
