/*
    Shutter class for Laserinterference
    Created by Timo Raab, October 13, 2021
    v0.1
*/

#ifndef Shutter_h
#define Shutter_h

#include "Arduino.h"
#include <Servo.h>

class Shutter {
    private:
        unsigned char _pin;
        int _shuttOff;
        int _shuttOn;
        Servo _servo;

    public:
        //constructor
        Shutter(unsigned char pin, int shuttOff, int shuttOn);

        //methods
        byte moveTo(int pos);
        byte shutterOpen();
        byte shutterClose();
        byte shutterAttach();
};

#endif

//EOF