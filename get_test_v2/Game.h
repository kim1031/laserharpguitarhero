#ifndef Game_h
#define Game_h

#include <string>
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

typedef struct laser_string 
{
    int LED_pin;
    int analog_pin;
} laser_string;

class Game
{
    private:
        static int score;
        static laser_string a_string;
        static laser_string s_string;
        static laser_string d_string;
        static laser_string f_string;

        static Adafruit_RA8875* tft;
        static DFRobotDFPlayerMini* mp3_player;

    public:
        static void setUp(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player);
        //static void homePage();
    private:
        static void getSong();
};

#endif