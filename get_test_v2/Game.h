#ifndef Game_h
#define Game_h

#include "LaserString.h"
#include "RectNote.h"
#include "Leaderboard.h"
#include "UsernameGetter.h"
#include "Home.h"
#include "SongSelection.h"

#include <string>
#include <string.h>
#include <WiFi.h>
#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>
#include <DFRobotDFPlayerMini.h>

#define a_LED_pin 13
#define s_LED_pin 12
#define d_LED_pin 14
#define f_LED_pin 26

#define a_analog_pin A7
#define s_analog_pin A6
#define d_analog_pin A3
#define f_analog_pin A0

class Game
{
    private:
        Home home_screen;
        
        int score;
        int state;

        Leaderboard leaderboard;
        
        LaserString a_string;
        LaserString s_string;
        LaserString d_string;
        LaserString f_string;

        RectNote a_rects[100];
        float num_a;
        int a_start;
        int a_end;
        RectNote s_rects[100];
        int s_start;
        int s_end;
        RectNote d_rects[100];
        int d_start;
        int d_end;
        RectNote f_rects[100];
        int f_start;
        int f_end;

        std::string user;
        std::string song_name;
        std::string artist_name;
        bool playing;
        int song;
        float song_len;

        char username[20];
        char building_username[20];
        UsernameGetter ug;

        SongSelection song_selection;
        
        Adafruit_RA8875* tft;
        DFRobotDFPlayerMini* mp3_player;

    public:
        //Game();
        Game(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player);
        void setUpLED();
        //void setUp(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player);
        //int getSongNum();
        void gamePlay(int elapsed, char* request_buffer, char* reponse_buffer);
        int getState();
        int getScore();
    private:
        void getSongData(char* request_buffer);
        void getSongList(char* request_buffer);
        void parseSongData(char* response_buffer, char* note_arr, float* note_time_arr, float* duration_arr);
        void extractTimes(char* note_arr, float* note_time_arr, float* duration_arr);
        void same_song();
        void new_round_same_user();
        void reset();
};

#endif
