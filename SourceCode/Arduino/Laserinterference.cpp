//Filename: Laserinterference.cpp

//______________________________________________________________________________
//_LIBRARIES____________________________________________________________________
//______________________________________________________________________________
  #include <Arduino.h>
  #include <Button.h>
  #include <EEPROM.h>
  #include <TaskScheduler.h>

  #include "Laserinterference.h"

  #include "./lib/buildSettings/compile/compileSettings.h"
  #include "./lib/buildSettings/pinConfiguration/pinConfiguration.h"
  #include "./lib/buildSettings/setupConfig/setupConfig.h"
  #include "./lib/menuInterface/menuInterface.h"
  #include "./lib/servoMotor/servoMotor.h"
  #include "./lib/motor/motorSerial.h"
  #include "./lib/procedures/procedures.h"


  #define TERMINATOR '\n'

//______________________________________________________________________________
//_PINS_AND_DEVICES_____________________________________________________________
//______________________________________________________________________________
  //_buttons____________________________________________________________________
  Button bRight = Button(buttonRight);
  Button bLeft = Button(buttonLeft);
  Button bUp = Button(buttonUp);
  Button bDown = Button(buttonDown);
  Button bOK = Button(buttonOK);
  Button bAbort = Button(buttonAbort);




//______________________________________________________________________________
//_INPUTS_______________________________________________________________________
//______________________________________________________________________________
  byte checkButtons() {
      if (bOK.isPressed(false)) return 0;
      if (bAbort.isPressed(false)) return 1;
      if (bUp.isPressed(false))  return 11;
      if (bDown.isPressed(false)) return 12;
      if (bLeft.isPressed(false)) return 13;
      if (bRight.isPressed(false)) return 14;
      return 255;
    }

  byte handleSerial() {
    if (Serial.available()) {
      String in = Serial.readStringUntil(TERMINATOR);
      if (in.length() == 1) { // menu test with keyboard
        switch (in.charAt(0)) {
          case 'e': return 0;
          case 'q': return 1;
          case 'w': return 11;
          case 's': return 12;
          case 'a': return 13;
          case 'd': return 14;
        }
      } else {
        return serialCommands(in);
      }
    }
    return 255; 
  } 

  byte serialCommands() {
    if (!Serial.available()) return 255;
    String in = Serial.readStringUntil(TERMINATOR);
    return serialCommands(in);
  }

  byte serialCommands(String in) {
      if (in.startsWith("turn")) { // turn test for motor connection

        byte tempA1[9]= {0,0,0,0,0,0,0,0,0};
        motorMoveCW(tempA1, 1, 6000);

      }

      if (in.startsWith("stop")) { // stop motor
        motorStop();
      }

      if (in.startsWith("mShut")) { // manual shutter
        in = in.substring(5);
        int tempShut = in.substring(0, 1).toInt();
        Shutter *tempShutter;
        switch(tempShut) {
          case 0: *tempShutter = shutter00;
          case 1: *tempShutter = shutter01;
        }
        in = in.substring(2);
        if (in.startsWith("Op")) shutterOpen(*tempShutter);
        if (in.startsWith("Cl")) shutterClose(*tempShutter);
        return 255;
      }

      if (in.startsWith("abo")) { //abort
        return 1;
      }

      if (in.startsWith("rep")) { // repeatable test
      //Syntax of String (square brackets coresponds to numbers in string)
      //linebreak in example only to fit pagewidth!
      //last newline (\n) is needed!
      //rep[amount of different angles],[motorApproach 1st angle],[amount of tests at 1st angle],[first angle],
      //[motorApproach 2nd angle],[amount of tests 2nd angle],[second angle],...,
      //[motorApproach nth angle],[amount of tests last angle],[last angle],\n
        lcd.setCursor(0,3);
        lcd.print("REP Test");
        in = in.substring(3);

        // get the number of queue elements and initialize array
        int ind = in.indexOf(",");
        String temp = in.substring(0,ind);
        int sizeOfQueue = temp.toInt();
        float queue[sizeOfQueue][3];
        for (int i=0; i<sizeOfQueue; i++) {
          queue[i][0] = 0;
          queue[i][1] = 0;
          queue[i][2] = 0;
        }
        in = in.substring(ind+1);

        // fill queue
        // first element is number of repeats, 2nd angle to move
        for (int i=0; i<sizeOfQueue*3; i++) {
          ind = in.indexOf(",");
          temp = in.substring(0,ind);
          queue[i/3][i%3] = temp.toInt();
          in = in.substring(ind+1);
        }

        //
        for (int i=0; i<sizeOfQueue; i++) { // iterate through different angle steps
          for (int j=0; j<(int)(queue[i][1]); j++) { // do test x times
            if (move2Angle(queue[i][2])) {
              break;
            }
            Serial.print("Angle: ");
            Serial.println(motorGetActualPosition());
            if (referenceStage(true, queue[i][0])) {
              break;
            }
            Serial.print("Ref: ");
            Serial.println(motorGetActualPosition());
            delay(500);
          }
        }
        lcd.clear();
        LCDML.DISP_update();
        lcdml_menu_display();
      }

      if (in.startsWith("resetEE")) {
        Serial.println("Reset EEPROM");
        EEPROM.put(0, standardAngle);
        copyArray(standardAngle, angle, 4);
        copyArray(standardAngle, angleTemp, 4);
        EEPROM.put(20, standardLaser);
        copyArray(standardLaser, laser, 4);
        copyArray(standardLaser, laserTemp, 4);
        EEPROM.put(4, standardTime);
        copyArray(standardTime, time, 4);
        copyArray(standardTime, timeTemp, 4);
        EEPROM.put(8, standardWait);
        copyArray(standardWait, wait, 4);
        copyArray(standardWait, waitTemp, 4);
        EEPROM.put(12, standardWaitTime);
        copyArray(standardWaitTime, waitTime, 4);
        copyArray(standardWaitTime, waitTimeTemp, 4);
        EEPROM.put(16, standardReference);
        copyArray(standardReference, reference, 4);
        copyArray(standardReference, referenceTemp, 4);
        Serial.println("Reset Finished");
      }
    return 255;
  }




//______________________________________________________________________________
//_INTERRUPT____________________________________________________________________
//______________________________________________________________________________
  // Set interrupt flag
  void isr2() {
    goInterrupt = true;
  }

  /*  activates/deactivates interlock functions
      input: 
  */
  void interruptFunction() {
    goInterrupt = false;
    if (digitalRead(interlockSystem) == LOW) {
      lcdBacklight(true);
      interlockStatus = true;
    } else {
      lcdBacklight(false);
      interlockStatus = false;
      shutterClose();
    }
  }




//______________________________________________________________________________
//_SETUP________________________________________________________________________
//______________________________________________________________________________
  void setup() {
    // serial init; only be needed if serial control is used
    Serial.begin(115200);                // start serial

    //Read EEProm
    setupConfigSetup();

    lcdSetup();
    menuSetup();
    shutterSetup();
    motorSetup();
    pinMode(2, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(interlockSystem),isr2, CHANGE);
    isr2();
    pinMode(ledCtrl, OUTPUT);
    digitalWrite(ledCtrl, HIGH);
  }

//______________________________________________________________________________
//_LOOP_________________________________________________________________________
//______________________________________________________________________________
  void loop() {
    if (goInterrupt) interruptFunction();
    startMenu();
    serialCommands();
  }

  //EOF