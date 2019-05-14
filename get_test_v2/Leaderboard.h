#ifndef Leaderboard_h
#define Leaderboard_h

#include "LaserString.h"

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
        Leaderboard();
        void setSongName(std::string song_name);
        void postToLeaderboard(char* request_buffer, int score, std::string user);
        void parseLeaderboard(char* response_buffer); //pass in response buffer
        void getLeaderboard(char* request_buffer);
        int displayScore(Adafruit_RA8875* tft, LaserString* string_1, LaserString* string_2, LaserString* string_3, LaserString* string_4, int score);
        int displayLeaderboard(Adafruit_RA8875* tft, LaserString* string_1, LaserString* string_2, LaserString* string_3);
};

#endif
