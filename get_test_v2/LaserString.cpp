#include "LaserString.h"

#include "Arduino.h"

LaserString::LaserString(int LED_pin, int analog_pin)
{
    this->LED_pin = LED_pin;
    this->analog_pin = analog_pin;

    pinMode(this->LED_pin, OUTPUT);
    digitalWrite(LED_pin, 0);
}

void LaserString::LEDControl(bool on)
{
    if (on)
        digitalWrite(LED_pin, 1);
    else if (!on)
        digitalWrite(LED_pin, 0);
}

//void LaserString::userAction()
//{
//    
//}
