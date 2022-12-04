//Filename: pinConfiguration.h

#ifndef PINCONFIGURATION_H
#define PINCONFIGURATION_H

//_Buttons______________________________________________________________________
const byte buttonRight = 7;
const byte buttonLeft = 6;
const byte buttonUp = 4;
const byte buttonDown = 5;
const byte buttonOK = A0;
const byte buttonAbort = A1;

//_Interlocks___________________________________________________________________
const byte endSwitch = 3;
const byte interlockSystem = 2;

//_Servos_______________________________________________________________________
const byte servoPin405 = 8;
const byte servoPin355 = 9;

//_MotorSerial__________________________________________________________________
const byte ctrlPin = 17;
const byte rxSoftware = 19;
const byte txSoftware = 18;

//_LED__________________________________________________________________________
const byte ledCtrl = 16;

#endif
//EOF