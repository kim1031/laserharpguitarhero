#ifndef Game_h
#define Game_h

#include "LaserString.h"
#include "RectNote.h"

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

#define a_LED_pin 12
#define s_LED_pin 13
#define d_LED_pin 27
#define f_LED_pin 26

#define a_analog_pin A0
#define s_analog_pin A11
#define d_analog_pin A6
#define f_analog_pin A7

class Game
{
    private:
        static int score;
        static int state;
        
        static LaserString a_string;
        static LaserString s_string;
        static LaserString d_string;
        static LaserString f_string;

        static RectNote a_rects[100];
        static int a_start;
        static int a_end;
        static RectNote s_rects[100];
        static int s_start;
        static int s_end;
        static RectNote d_rects[100];
        static int d_start;
        static int d_end;
        static RectNote f_rects[100];
        static int f_start;
        static int f_end;

        static std::string user;
        static std::string song_name;
        static std::string artist_name;
        static int song;
        static float song_len;

        static Adafruit_RA8875* tft;
        static DFRobotDFPlayerMini* mp3_player;

    public:
        static void setUp(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player);
        //static int getSongNum();
        static void gamePlay();
    private:
        static void getSongData(char* request_buffer);
        static void parseSongData(string data_to_process, char* note_arr, float* note_time_arr, float* duration_arr);
        static void extractTimes(char* note_arr, float* note_time_arr, float* duration_arr);
};

#endif