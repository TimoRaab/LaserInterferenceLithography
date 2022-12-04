//Filename: menuFunctions.cpp

#include <LCDMenuLib2.h>
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>
#include <TaskSchedulerDeclarations.h>

#include "menuInterface.h"
#include "../buildSettings/compile/compileSettings.h"
#include "../buildSettings/pinConfiguration/pinConfiguration.h"
#include "../buildSettings/setupConfig/setupConfig.h"
#include "../servoMotor/servoMotor.h"
#include "../procedures/procedures.h"
#include "../../Laserinterference.h"
#include "../motor/motorSerial.h"

//Change Angle
void changeAngleSetting(byte line) {
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Save w OK");
  lcd.setCursor(0, 3);
  lcd.print("Discard w Abort");
  lcd.setCursor(0,0);
  lcd.print("Edit Angle");
  byte cursorStart = 1;
  lcd.setCursor(cursorStart,1);
  lcd.print(angleArr2String(angleTemp));
  lcd.setCursor(cursorStart,1);
  byte position = 0;
  LCDML.BT_resetAll();
  while (true) {
    runner.execute();
    lcd.cursor();
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        copyArray(angleTemp, angle, 4);
        EEPROM.put(0, angleTemp);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkQuit()) {
        copyArray(angle, angleTemp, 4);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkUp()) {
        angleTemp[position] += ((maxAngle[position]+1)+1);
        angleTemp[position] = angleTemp[position]%(maxAngle[position]+1);
        lcd.setCursor(cursorStart,1);
        lcd.print(angleArr2String(angleTemp));
      }
      if (LCDML.BT_checkDown()) {
        angleTemp[position] += ((maxAngle[position]+1)-1);
        angleTemp[position] = angleTemp[position]%(maxAngle[position]+1);
        lcd.setCursor(cursorStart,1);
        lcd.print(angleArr2String(angleTemp));
      }
      if (LCDML.BT_checkLeft()) {
        if (position != 0) {
          position--;
        }
      }
      if (LCDML.BT_checkRight()) {
        if (position < 3) {
          position++;
        }
      }
      lcd.setCursor(cursorStart + anglePositions[position], 1);
      LCDML.BT_resetAll();
    }
  }
}


void changeTimeSetting(byte line) {
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Save w OK");
  lcd.setCursor(0, 3);
  lcd.print("Discard w Abort");
  lcd.setCursor(0,0);
  lcd.print("Edit Time");
  byte cursorStart = 1;
  lcd.setCursor(cursorStart,1);
  lcd.print(timeArr2String(timeTemp));
  lcd.setCursor(cursorStart,1);
  byte position = 0;
  lcd.cursor();
  LCDML.BT_resetAll();
  while (true) {
    runner.execute();
    lcd.cursor();
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        copyArray(timeTemp, time, 4);
        EEPROM.put(4, timeTemp);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkQuit()) {
        copyArray(time, timeTemp, 4);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkUp()) {
        timeTemp[position] += ((maxTime[position]+1)+1);
        timeTemp[position] = timeTemp[position]%(maxTime[position]+1);
        lcd.setCursor(cursorStart,1);
        lcd.print(timeArr2String(timeTemp));
      }
      if (LCDML.BT_checkDown()) {
        timeTemp[position] += ((maxTime[position]+1)-1);
        timeTemp[position] = timeTemp[position]%(maxTime[position]+1);
        lcd.setCursor(cursorStart,1);
        lcd.print(timeArr2String(timeTemp));
      }
      if (LCDML.BT_checkLeft()) {
        if (position != 0) {
          position--;
        }
      }
      if (LCDML.BT_checkRight()) {
        if (position < 3) {
          position++;
        }
      }
      lcd.setCursor(cursorStart + timePositions[position], 1);
      LCDML.BT_resetAll();
    }
  }
}


void changeWaitSetting(byte line) {
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Save w OK");
  lcd.setCursor(0, 3);
  lcd.print("Discard w Abort");
  lcd.setCursor(0,0);
  lcd.print("Edit Wait");
  byte cursorStart = 1;
  lcd.setCursor(cursorStart,1);
  lcd.print(waitArr2String(waitTemp));
  lcd.setCursor(cursorStart,1);
  LCDML.BT_resetAll();
  byte position = 3;
  while (true) {
    runner.execute();
    lcd.cursor();
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        copyArray(waitTemp, wait, 4);
        EEPROM.put(8,waitTemp);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkQuit()) {
        copyArray(wait, waitTemp, 4);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkUp()) {
        waitTemp[position] += ((maxWait[position]+1)+1);
        waitTemp[position] = waitTemp[position]%(maxWait[position]+1);
        lcdClearLine(1);
        lcd.setCursor(cursorStart,1);
        lcd.print(waitArr2String(waitTemp));
      }
      if (LCDML.BT_checkDown()) {
        waitTemp[position] += ((maxWait[position]+1)-1);
        waitTemp[position] = waitTemp[position]%(maxWait[position]+1);
        lcdClearLine(1);
        lcd.setCursor(cursorStart,1);
        lcd.print(waitArr2String(waitTemp));
      }
      LCDML.BT_resetAll();
    }
  }
}


void changeWaitTimeSetting(byte line) {
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Save w OK");
  lcd.setCursor(0, 3);
  lcd.print("Discard w Abort");
  lcd.setCursor(0,0);
  lcd.print("Edit Wait Time");
  byte cursorStart = 1;
  lcd.setCursor(cursorStart,1);
  lcd.print(timeArr2String(waitTimeTemp));
  lcd.setCursor(cursorStart,1);
  LCDML.BT_resetAll();
  byte position = 0;
  while (true) {
    runner.execute();
    lcd.cursor();
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        copyArray(waitTimeTemp, waitTime, 4);
        EEPROM.put(12, waitTimeTemp);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkQuit()) {
        copyArray(waitTime, waitTimeTemp, 4);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkUp()) {
        waitTimeTemp[position] += ((maxWaitTime[position]+1)+1);
        waitTimeTemp[position] = waitTimeTemp[position]%(maxWaitTime[position]+1);
        lcd.setCursor(cursorStart,1);
        lcd.print(timeArr2String(waitTimeTemp));
      }
      if (LCDML.BT_checkDown()) {
        waitTimeTemp[position] += ((maxWaitTime[position]+1)-1);
        waitTimeTemp[position] = waitTimeTemp[position]%(maxWaitTime[position]+1);
        lcd.setCursor(cursorStart,1);
        lcd.print(timeArr2String(waitTimeTemp));
      }
      if (LCDML.BT_checkLeft()) {
        if (position != 0) {
          position--;
        }
      }
      if (LCDML.BT_checkRight()) {
        if (position < 3) {
          position++;
        }
      }
      lcd.setCursor(cursorStart + waitTimePositions[position], 1);
      LCDML.BT_resetAll();
    }
  }
}


void changeReferenceSetting(byte line) {
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Save w OK");
  lcd.setCursor(0, 3);
  lcd.print("Discard w Abort");
  lcd.setCursor(0,0);
  lcd.print("Edit Referencing");
  byte cursorStart = 1;
  lcd.setCursor(cursorStart,1);
  lcd.print(referenceArr2String(referenceTemp));
  lcd.setCursor(cursorStart,1);
  LCDML.BT_resetAll();
  byte position = 3;
  while (true) {
    runner.execute();
    lcd.cursor();
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        copyArray(referenceTemp, reference, 4);
        EEPROM.put(16,referenceTemp);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkQuit()) {
        copyArray(reference, referenceTemp, 4);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkUp()) {//up
        referenceTemp[position] += ((maxReference[position]+1)+1);
        referenceTemp[position] = referenceTemp[position]%(maxReference[position]+1);
        lcdClearLine(1);
        lcd.setCursor(cursorStart,1);
        lcd.print(referenceArr2String(referenceTemp));
      }
      if (LCDML.BT_checkDown()) {//down
        referenceTemp[position] += ((maxReference[position]+1)-1);
        referenceTemp[position] = referenceTemp[position]%(maxReference[position]+1);
        lcdClearLine(1);
        lcd.setCursor(cursorStart,1);
        lcd.print(referenceArr2String(referenceTemp));
      }
      LCDML.BT_resetAll();
    }
  }
}


void manual_MotorAngle(byte line) {
  byte tempArr[4];
  byte tempA[9] = {0,0,0,0,0,0,0,0,0};
  copyArray(angle, tempArr, 4);
  byte cursorStart = 7;
  byte position = 0;
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Move Motor to Angle");
  lcd.setCursor(0,1);
  lcd.print(String("Angle: ") + angleArr2String(tempArr));
  lcd.setCursor(0,2);
  lcd.print("Move with OK");
  lcd.setCursor(0,3);
  lcd.print("Quit with Abort");
  LCDML.BT_resetAll();
  runner.execute();

  while (!LCDML.BT_checkQuit()) {
    lcd.cursor();
    if (LCDML.BT_checkEnter()) {
      long pos = angleArr2Float(tempArr)*256*200/3;
      pos = -pos;
      motorMove2Position(tempA, 1, MOTOR_MVP_ABSOLUTE, pos);
    }
    if (LCDML.BT_checkUp()) {
      tempArr[position] += ((maxAngle[position]+1)+1);
      tempArr[position] = tempArr[position]%(maxAngle[position]+1);
      lcd.setCursor(cursorStart,1);
      lcd.print(angleArr2String(tempArr));
    }
    if (LCDML.BT_checkDown()) {
      tempArr[position] += ((maxAngle[position]+1)-1);
      tempArr[position] = tempArr[position]%(maxAngle[position]+1);
      lcd.setCursor(cursorStart,1);
      lcd.print(angleArr2String(tempArr));
    }
    if (LCDML.BT_checkLeft()) {//left
      if (position != 0) {
        position--;
      }
    }
    if (LCDML.BT_checkRight()) {//right
      if (position < 3) {
        position++;
      }
    }
    lcd.setCursor(cursorStart + anglePositions[position], 1);
    LCDML.BT_resetAll();
    runner.execute();
  }
  motorStop();
}


void changeLaserSetting(byte line) {
  lcd.clear();
  lcd.setCursor(0, 2);
  lcd.print("Save w OK");
  lcd.setCursor(0, 3);
  lcd.print("Discard w Abort");
  lcd.setCursor(0,0);
  lcd.print("Edit Wait");
  byte cursorStart = 1;
  lcd.setCursor(cursorStart,1);
  lcd.print(laserArr2String(laserTemp));
  lcd.setCursor(cursorStart,1);
  LCDML.BT_resetAll();
  byte position = 3;
  while (true) {
    runner.execute();
    lcd.cursor();
    if (LCDML.BT_checkAny()) {
      if (LCDML.BT_checkEnter()) {
        copyArray(laserTemp, laser, 4);
        EEPROM.put(20,laserTemp);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkQuit()) {
        copyArray(laser, laserTemp, 4);
        lcd.cursor_off();
        LCDML.BT_resetAll();
        return;
      }
      if (LCDML.BT_checkUp()) {
        laserTemp[position] += ((maxLaser[position]+1)+1);
        laserTemp[position] = laserTemp[position]%(maxLaser[position]+1);
        lcdClearLine(1);
        lcd.setCursor(cursorStart,1);
        lcd.print(laserArr2String(laserTemp));
      }
      if (LCDML.BT_checkDown()) {
        laserTemp[position] += ((maxLaser[position]+1)-1);
        laserTemp[position] = laserTemp[position]%(maxLaser[position]+1);
        lcdClearLine(1);
        lcd.setCursor(cursorStart,1);
        lcd.print(laserArr2String(laserTemp));
      }
      LCDML.BT_resetAll();
    }
  }
}