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
    this->LED_pin = LED_pin;
    this->analog_pin = analog_pin;
    this->scored = true;
    this->hand_in = false;
    this->ref_hand_in_time = 0;
    this->actual_hand_in_time = 0;
    this->feedback_tag = "";
    this->feedback_time = 0;
}

void LaserString::beginLights()
{
    pinMode(LED_pin, OUTPUT);
    digitalWrite(LED_pin, 0);
}

void LaserString::LEDControl(bool on)
{
    if (on)
        digitalWrite(LED_pin, 1);
    else if (!on)
        digitalWrite(LED_pin, 0);
}

bool LaserString::broken()
{
    int bins = analogRead(analog_pin);
    float voltage = (bins / 4096.0) * 3.3;
    return (voltage >= 1.5);
}

void LaserString::setRefTime(int time_ms)
{
    ref_hand_in_time = time_ms;
}

int LaserString::getRef()
{
    return actual_hand_in_time;
}

void LaserString::toScoreNote()
{
    scored = false;
}

void LaserString::userAction()
{
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
    int time_diff = fabs(actual_hand_in_time - ref_hand_in_time);
    int add_score = 0;
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
    tft->textTransparent(RA8875_RED);
    if (time_ms - feedback_time <= 500)
    {
        tft->textSetCursor(text_loc, 400);
        tft->textWrite(feedback_tag.c_str());
    }
}

void LaserString::reset()
{
    this->scored = true;
    this->hand_in = false;
    this->ref_hand_in_time = 0;
    this->actual_hand_in_time = 0;
    this->feedback_tag = "";
    this->feedback_time = 0;
}
