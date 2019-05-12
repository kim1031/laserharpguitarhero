#include "Game.h"

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


static void Game::setUp(Adafruit_RA8875* input_tft, DFRobotDFPlayerMini* input_mp3_player) 
{
    tft = input_tft;
    mp3_player = input_tft;

    a_string.LED_pin = a_LED_pin;
    a_string.analog_pin = a_analog_pin;
    s_string.LED_pin = s_LED_pin;
    s_string.analog_pin = s_analog_pin;
    d_string.LED_pin = d_LED_pin;
    d_string.analog_pin = d_analog_pin;
    f_string.LED_pin = f_LED_pin;
    f_string.analog_pin = f_analog_pin;

    pinMode(a_string.LED_pin, OUTPUT);
    pinMode(s_string.LED_pin, OUTPUT);
    pinMode(d_string.LED_pin, OUTPUT);
    pinMOde(f_string.LED_pin, OUTPUT);

    digitalWrite(a_string.LED_pin, 0);
    digitalWrite(s_string.LED_pin, 0);
    digitalWrite(d_string.LED_pin, 0);
    digitalWrite(f_string.LED_pin, 0);

    tft->fillRect(0, 380, 800, 100, RA8875_WHITE);

    getSong();
}





static void Game::getSong();