/*
    Shutter class for Laserinterference
    Created by Timo Raab, October 13, 2021
    v0.1
*/

#include "Arduino.h"
#include <Servo.h>
#include "Shutter.h"

//Constructor
Shutter::Shutter(unsigned char pin, int shuttOff, int shuttOn) {
    _pin = pin;
    _shuttOff = shuttOff;
    _shuttOn = shuttOn;

}

byte Shutter::moveTo(int pos) {
    _servo.writeMicroseconds(pos);
    return 0;
}

byte Shutter::shutterOpen() {
    moveTo(_shuttOn);
    return 0;
}

byte Shutter::shutterClose() {
    moveTo(_shuttOff);
    return 0;
}

byte Shutter::shutterAttach() {
    _servo.attach(_pin);
    return 0;
}