//Filename: setupConfig.cpp

#include <Arduino.h>
#include <EEPROM.h>

#include "setupConfig.h"
#include "../compile/compileSettings.h"

byte angle[4];
byte laser[4]; // = {0,0,0,0};
byte time[4]; // = {0,0,1,0};
byte wait[4]; // = {0,0,0,1};
byte waitTime[4]; // = {0,0,0,5};
byte reference[4]; // = {0,0,0,0};

byte angleTemp[4];
byte laserTemp[4];
byte timeTemp[4];
byte waitTemp[4];
byte waitTimeTemp[4]; 
byte referenceTemp[4];

byte maxAngle[4] = {9,9,9,9};
byte maxLaser[4] = {0,0,0,1};
byte maxTime[4] = {9,5,9,9};
byte maxWait[4] = {0,0,0,2};
byte maxWaitTime[4] = {9,5,9,9};
byte maxReference[4] = {0,0,0,1};

byte standardAngle[4] = {0,0,2,0};
byte standardLaser[4] = {0,0,0,0};
byte standardTime[4] = {0,3,0,0};
byte standardWait[4] = {0,0,0,1};
byte standardWaitTime[4] = {0,0,5,0};
byte standardReference[4] = {0,0,0,0};

byte anglePositions[4] = {0,1,3,4};
byte laserPositions[4] = {0,0,0,0};
byte timePositions[4] = {0,2,3,5};
byte waitPositions[4] = {0,0,0,0};
byte waitTimePositions[4] = {0,2,3,5};
byte referencePositions[4] = {0,0,0,0};


boolean motorAttached = true;
long motorZero = 0;

uint16_t motorReferenceApproach = 3;

void setupConfigSetup() {
  if (!FIRSTInstall) { // when installing first on a new device, EEProm has to be initialized
    EEPROM.get(0, angle);
    EEPROM.get(4, time);
    EEPROM.get(8, wait);
    EEPROM.get(12, waitTime);
    EEPROM.get(16, reference);
    EEPROM.get(20, laser);
  }
}

//EOF