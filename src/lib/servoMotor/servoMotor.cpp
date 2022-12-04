//Filename: servo.cpp

#include <Arduino.h>
#include <Servo.h>
#include <Shutter.h>
#include "servoMotor.h"
#include "../buildSettings/pinConfiguration/pinConfiguration.h"
#include "../buildSettings/compile/compileSettings.h"
//#include "../setupConfig/setupConfig.h"
#include "../../Laserinterference.h"

const int shuttOff00 = 1850;
const int shuttOn00 = 1500;

const int shuttOff01 = 1500;
const int shuttOn01 = 1850;

Shutter shutter00(servoPin405, shuttOff00, shuttOn00); //405nm
Shutter shutter01(servoPin355, shuttOff01, shuttOn01); //355nm


/* opens shutter
    Note:  Interlock has to be closed
*/
void shutterOpen(Shutter shut) {
  if (interlockStatus)
    shut.shutterOpen();
}

/* close shutter
*/
void shutterClose(Shutter shut) {
  shut.shutterClose();
}

/*closes all shutter
    Note:  is called when interlock gets opened (except manual shutter mode)
*/
void shutterClose() {
  shutterClose(shutter00);
  shutterClose(shutter01);
}


/* initialize shutter
*/
void shutterSetup() {
  shutter00.shutterAttach();
  shutter01.shutterAttach();
  shutterClose(shutter00);
  shutterClose(shutter01);
}

//EOF