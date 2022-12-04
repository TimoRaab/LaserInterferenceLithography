//Filename: procedures.cpp

#include <Arduino.h>
#include <Button.h>

#include "procedures.h"
#include "./lib/buildSettings/compile/compileSettings.h"
#include "./lib/buildSettings/pinConfiguration/pinConfiguration.h"
#include "./lib/buildSettings/setupConfig/setupConfig.h"

#include "../servoMotor/servoMotor.h"
#include "../motor/motorSerial.h"
#include "../menuInterface/menuInterface.h"
#include "../../Laserinterference.h"

extern byte checkButtons();
extern byte handleSerial();

Button bEnd = Button(endSwitch, 1, 0, 0, nullFunction, 0);


/* end of exposure
    Note: get called every time exposure is finished or aborted
*/
void stopExposure() {
  shutterClose();
  motorStop();
}


/* moves to correct position for exposure
    input: angle in degree
    output: status byte if positioning worked (=0) or aborted (=255)
    note: manual interlock checking
    */
byte move2Angle(float degree){
    // correct for backlash
    long pos = (degree-2)*256*200/3;
    pos = -pos; // inverted to move in correct direction
    byte tempA1[9] = {0,0,0,0,0,0,0,0,0};
    motorMove2Position(tempA1, 1, MOTOR_MVP_ABSOLUTE, pos);

    if (motorAttached) {
      while (abs(pos - motorGetActualPosition()) > 1) { // wait till move is finished
        if (checkButtons() == 1 || handleSerial() == 1) {
          stopExposure();
          return 255;
        }
      } 
    }

    // go to correct position
    pos = degree*256*200/3;
    pos = -pos; // inverted to move in correct direction
    motorMove2Position(tempA1, 1, MOTOR_MVP_ABSOLUTE, pos);

    if (motorAttached) {
      while (abs(pos - motorGetActualPosition()) > 1) { // wait till move is finished
        if (checkButtons() == 1 || handleSerial() == 1) {
          stopExposure();
          return 255;
        }
      } 
    }
    delay(200);
    motorStop();
    return 0;
}


/* exposure routine
    input:  angle in degree
            exposure time in s
            wait type
            wait time in s
            reference type
    output: status byte for exposure (finished = 0, aborted = 255)
    note: manual interlock management
*/
byte startExposure(float degree, unsigned long time, byte wait, unsigned long waitTime, byte reference) {
  lcdClear();

  lcdPrint(0,0,"Step: POSITIONING");
  lcdPrint(0,1,"Angle: " + angle2String(degree));
  lcdPrint(0,2,"Time: " + time2String(time));
  lcdPrint(0,3,"Wait: " + time2String(waitTime));
  

  unsigned long startTime = millis();
  //Reference if wanted
  if (reference == refTrue) {
    if (referenceStage(false, motorReferenceApproach) == 255) {
      stopExposure();
      return 255;
    }
  }

  //Move to correct position
  if (motorAttached) {  
    if (move2Angle(degree) == 255) {
      stopExposure();
      return 255;
    }
  }
  unsigned long timeLeft = (waitTime - (millis() - startTime));

  if (wait) {
    lcdClearLine(0);
    lcdPrint(0,0,"Step: WAITING");
    if (wait == waitExc) {
      startTime = millis();
    }
    while ((millis() - startTime) < waitTime) {
      if (digitalRead(interlockSystem) == HIGH) {
        stopExposure();
        return 255;
      }
      unsigned long timeLeftTemp = (waitTime - (millis() - startTime));
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }

      if (timeLeftTemp != timeLeft) {
        timeLeft = timeLeftTemp;
        lcdPrint(6,3,time2String(timeLeft));
      }
    }
  }
  lcdPrint(6,3,time2String(0));

  lcdClearLine(0);
  lcdPrint(0,0,"Step: EXPOSURE");
  long laserTemp = arr2Long(laser);
  switch (laserTemp) {
    case 0: shutterOpen(shutter00); break;
    case 1: shutterOpen(shutter01); break;
  }
  startTime = millis();
  timeLeft = (time - (millis() - startTime));

  while ((millis() - startTime) < time) {
    unsigned long timeLeftTemp = (time - (millis() - startTime));
    if (digitalRead(interlockSystem) == HIGH) {
      stopExposure();
      return 255;
    }
    if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
    }
    if (timeLeftTemp != timeLeft) {
      timeLeft = timeLeftTemp;
      lcdPrint(6,2,time2String(timeLeft));
      }
    }
  stopExposure();
  return 0;
}



/* reference routine
    input:  repTest         true if used as a repeat test, does not set motor position at end
            approachCount   how often the end switches are approached
    output: status byte for reference (reference found = 0, reference not found = 255)
    note: manual interlock management
*/
byte referenceStage(boolean repTest, uint16_t approachCount) {
  lcdClear();

  lcdPrint(0,0,"REFERENCING");
  lcdPrint(0,1,"Please Wait");
  lcdPrint(0,2,"Find 1st switch");

  byte tempA1[9] = {0,0,0,0,0,0,0,0,0};
  long pos1 = 0;

  // find first switch side roughly
  if (bEnd.isPressed(false)) { // switch not pressed
    motorMoveCCW(tempA1, 1, 51200); //1 motor rps
    while (bEnd.isPressed(false)) {
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }
    }
  } else { // switch already pressed
    motorMoveCW(tempA1, 1, 51200); //1 motor rps
    while (!bEnd.isPressed(false)) { //wait till end switch is activated or aborted
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }
    }
    motorStop();
  }

  for (uint16_t i=0; i<approachCount; i++) {
    // deactivate switch
    motorMoveCW(tempA1, 1, 6400);
    while (!bEnd.isPressed(false)) {
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }
    }
    motorStop();

    // find switch position
    motorMoveCCW(tempA1, 1, 3200); // 0.1875 degps
    while (bEnd.isPressed(false)) { //wait till end switch is activated or aborted
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }
    }
    delay(500);
    motorStop();
    pos1 += motorGetActualPosition();
    if (repTest) {
      Serial.print(pos1);
      Serial.print(",");
    }
  }
  if (repTest) Serial.println("");

  // 1st postion found!
  pos1 = pos1/approachCount;
  if (repTest) {
    Serial.print("Pos1: ");
    Serial.println(pos1);
  }

  lcdPrint(0,2, "Find 2nd switch");

  // find 2nd switch side roughly (wrong direction!)
  motorMoveCCW(tempA1, 1, 51200); //1 motor rps
  while (!bEnd.isPressed(false)) { //wait till end switch is deactivated or aborted
    if (checkButtons() == 1 || handleSerial() == 1) {
      stopExposure();
      return 255;
    }
  }
  motorStop();

  // find 2nd switch 
  motorMoveCW(tempA1, 1, 3200);
  while (bEnd.isPressed(false)) { //wait till end switch is activated or aborted
    if (checkButtons() == 1 || handleSerial() == 1) {
      stopExposure();
      return 255;
    }
  }
  delay(500);
  motorStop();

  long pos2 = motorGetActualPosition();
  if (repTest) {
    Serial.print(pos2);
    Serial.print(",");
  }


  for (uint16_t i=1; i<approachCount; i++) { // modified for first movement
    // deactivate switch
    motorMoveCCW(tempA1, 1, 6400);
    while (!bEnd.isPressed(false)) {
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }
    }
    motorStop();

    // find switch position
    motorMoveCW(tempA1, 1, 3200); // 0.1875 degps
    while (bEnd.isPressed(false)) { //wait till end switch is activated or aborted
      if (checkButtons() == 1 || handleSerial() == 1) {
        stopExposure();
        return 255;
      }
    }
    delay(500);
    motorStop();
    pos2 += motorGetActualPosition();
    if (repTest) {
      Serial.print(pos2);
      Serial.print(",");
    }
  }

  if (repTest) Serial.println("");
  pos2 = pos2/approachCount;
  if (repTest) {
    Serial.print("Pos2: ");
    Serial.println(pos2);
  }

  motorZero = (pos1+pos2); 
  if (motorZero%2 == 1) motorZero--;
  motorZero = motorZero/2;

  lcdClearLine(2);
  lcdPrint(0, 2, "Switches found!");
  lcdPrint(0, 3, "Move 2 Zero");
  motorMove2Position(tempA1, 1, 0, motorZero);
  while (abs(motorZero - motorGetActualPosition()) > 1) { // wait till move is finished
    if (checkButtons() == 1 || handleSerial() == 1) {
      stopExposure();
      return 255;
    }
  } 
  
  delay(500);
  motorStop();
  if (!repTest) motorSetActualPosition(0);
  return 0;
}
//EOF