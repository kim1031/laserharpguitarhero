/*
 * Class which represents a "string" of our laser harp, including the LED that represents it 
 * and the phototransistor which takes in input about when the string is "plucked" or broken.
 * Also handles scoring of user actions.
 */
 
#include "LaserString.h"

#include "Arduino.h"
#include <gfxfont.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SPITFT.h>
#include <Adafruit_SPITFT_Macros.h>
#include <Adafruit_RA8875.h>
#include <SPI.h>

LaserString::LaserString(int LED_pin, int analog_pin)
{
    //sets up both the pin controlling the LED and the pin controlling the photoresistor
    //also initializes attributes important to other functionality.
    this->LED_pin = LED_pin;
    this->analog_pin = analog_pin;
    this->scored = true; //whether user action has been scored or not yet
    this->hand_in = false;
    this->ref_hand_in_time = 0; //reference time is when the string should be broken according to the beatmap
    this->actual_hand_in_time = 0; //records when the laser actually does get broken in real life
    this->feedback_tag = "";
    this->feedback_time = 0;
}

void LaserString::beginLights()
{
    //makes LED usable/able to turn on upon command
    pinMode(LED_pin, OUTPUT);
    digitalWrite(LED_pin, 0);
}

void LaserString::LEDControl(bool on)
{
    //turns LED on and off
    if (on)
        digitalWrite(LED_pin, 1);
    else if (!on)
        digitalWrite(LED_pin, 0);
}

bool LaserString::broken()
{
    //checks whether the string has been broken
    int bins = analogRead(analog_pin);
    float voltage = (bins / 4096.0) * 3.3;
    return (voltage >= 1.5);
}

void LaserString::setRefTime(int time_ms)
{
    ref_hand_in_time = time_ms;
}


void LaserString::toScoreNote() 
{
    scored = false;
}

void LaserString::userAction()
{
    //read in and record user action
    int bins = analogRead(analog_pin);
    float voltage = (bins / 4096.0) * 3.3;
    if (voltage >= 1.5 && !hand_in)
    {
        actual_hand_in_time = millis();
        hand_in = true;
    } else if (voltage < 1.5)
        hand_in = false;
}

int LaserString::scoreAction(int time_ms)
{
    //if the current user action available hasn't yet been scored, score it
    int time_diff = fabs(actual_hand_in_time - ref_hand_in_time);
    int add_score = 0;
    
    //calculates how much score boost a user gets based on accuracy
    if (!scored)
    {
        feedback_time = time_ms;
        if (time_diff <= 10)
        {
            feedback_tag = "Perfect";
            add_score = 5;
        } else if (time_diff <= 25)
        {
            feedback_tag = "Great";
            add_score = 3;
        } else if (time_diff <= 50)
        {
            feedback_tag = "Good";
            add_score = 2;
        } else if (time_diff <= 100)
        {
            feedback_tag = "OK";
            add_score = 1;
        } else
        {
            feedback_tag = "Miss";
            add_score = 0;
        }
    }
    scored = true;
    return add_score;
}

void LaserString::displayFeedback(int time_ms, Adafruit_RA8875* tft, int text_loc)
{
    //display feedback about the user's accuracy for half a second
    tft->textTransparent(RA8875_RED);
    if (time_ms - feedback_time <= 500)
    {
        tft->textSetCursor(text_loc, 400);
        tft->textWrite(feedback_tag.c_str());
    }
}

void LaserString::reset()
{
    //reset all aspects related to scoring
    this->scored = true;
    this->hand_in = false;
    this->ref_hand_in_time = 0;
    this->actual_hand_in_time = 0;
    this->feedback_tag = "";
    this->feedback_time = 0;
}
