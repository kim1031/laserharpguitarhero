#ifndef Leaderboard_h
#define Leaderboard_h

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <string>
//using namespace std;

class Leaderboard {
    private:
        int scores[20];
        std::string users[20];
        std::string song_name;
        int array_size;

    public:
        Leaderboard(std::string song_name);
        void parse_leaderboard(std::string str); //pass in response buffer
        void get_leaderboard(char* request_buffer);
        void display_leaderboard(Adafruit_RA8875* tft);
};

#endif
