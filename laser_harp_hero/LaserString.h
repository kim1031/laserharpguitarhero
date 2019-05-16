#ifndef LaserString_h
#define LaserString_h

#include "Arduino.h"
#include <string>
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

class LaserString
{
    private:
        int LED_pin;
        int analog_pin;
        bool scored;
        bool hand_in;
        int ref_hand_in_time;
        int actual_hand_in_time;
        std::string feedback_tag;
        int feedback_time;
    public:
        LaserString(int LED_pin, int analog_pin);
        void beginLights();
        void LEDControl(bool on);
        bool broken();
        void setRefTime(int time_ms);
        void toScoreNote();
        void userAction();
        int getRef();
        int scoreAction(int time_ms);  //text_loc = horizontal location of text, under rectangle notes of corresponding string
        void displayFeedback(int time_ms, Adafruit_RA8875* tft, int text_loc);
        void reset();
};

#endif
