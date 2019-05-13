#ifndef LaserString_h
#define LaserString_h

#include "Arduino.h"

class LaserString
{
    private:
        int LED_pin;
        int analog_pin;
    public:
        LaserString(int LED_pin, int analog_pin);
        void LEDControl(bool on);
        //int userAction();
};

#endif
