//Filename: servo.h

#ifndef SERVOMOTOR_H
#define SERVOMOTOR_H

#include <Shutter.h>

extern Shutter shutter00;
extern Shutter shutter01;

void shutterOpen(Shutter shut);
void shutterClose(Shutter shut);

void shutterClose();

void shutterSetup();

#endif
//EOF