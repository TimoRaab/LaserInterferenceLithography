//Filename: menuInterface.cpp

#include <LCDMenuLib2.h>
#include <LiquidCrystal_I2C.h>

#include "menuInterface.h"
#include "menuFunctions.h"
#include "../buildSettings/compile/compileSettings.h"
#include "../buildSettings/pinConfiguration/pinConfiguration.h"
#include "../buildSettings/setupConfig/setupConfig.h"
#include "../servoMotor/servoMotor.h"
#include "../procedures/procedures.h"
#include "../../Laserinterference.h"
#include "../motor/motorSerial.h"

//_Settings_____________________________________________________________________
  //_common_____________________________________________________________________
  #define _LCDML_DISP_cols  20
  #define _LCDML_DISP_rows  4

  #define _LCDML_DSIP_use_header 0

  #define _LCDML_DISP_cfg_cursor                     0x7E   // cursor Symbol
  #define _LCDML_DISP_cfg_scrollbar                  1      // enable a scrollbar

  const uint8_t scroll_bar[5][8] = {
    {B10001, B10001, B10001, B10001, B10001, B10001, B10001, B10001}, // scrollbar top
    {B11111, B11111, B10001, B10001, B10001, B10001, B10001, B10001}, // scroll state 1
    {B10001, B10001, B11111, B11111, B10001, B10001, B10001, B10001}, // scroll state 2
    {B10001, B10001, B10001, B10001, B11111, B11111, B10001, B10001}, // scroll state 3
    {B10001, B10001, B10001, B10001, B10001, B10001, B11111, B11111}  // scrollbar bottom
  };

  
  byte common = 0;
  boolean dyn_menu_is_displayed = false;

//_external_functions___________________________________________________________
  extern uint8_t checkButtons();
  extern uint8_t handleSerial();
  extern void isr2();


//_internal_functions_forward___________________________________________________


//_Task_Scheduler_______________________________________________________________
Scheduler runner;
Task checkInputs(20, TASK_FOREVER, &lcdml_menu_control);

//_Menu_Initialization__________________________________________________________
LCDMenuLib2_menu LCDML_0 (255, 0, 0, NULL, NULL); // root menu element (do not change)
LCDMenuLib2 LCDML(LCDML_0, 
                  _LCDML_DISP_rows, 
                  _LCDML_DISP_cols, 
                  lcdml_menu_display, 
                  lcdml_menu_clear, 
                  lcdml_menu_control);
LiquidCrystal_I2C lcd(0x27,_LCDML_DISP_cols,_LCDML_DISP_rows);


//_Menu_Configuration___________________________________________________________
LCDML_addAdvanced (0 , LCDML_0         , 1  , NULL, "Start", menu_startExposure,0,_LCDML_TYPE_default);
LCDML_addAdvanced (1 , LCDML_0         , 2  , NULL, "", menu_setLaser,0, _LCDML_TYPE_dynParam);
LCDML_addAdvanced (2 , LCDML_0         , 3  , NULL, "", menu_setAngle,0,_LCDML_TYPE_dynParam);
LCDML_addAdvanced (3 , LCDML_0         , 4  , NULL, "", menu_setTime,0,_LCDML_TYPE_dynParam);
LCDML_addAdvanced (4 , LCDML_0         , 5  , NULL, "", menu_setWait,0,_LCDML_TYPE_dynParam);
LCDML_addAdvanced (5 , LCDML_0         , 6  , NULL, "", menu_setWaitTime,0,_LCDML_TYPE_dynParam);
LCDML_addAdvanced (6 , LCDML_0         , 7  , NULL, "", menu_reference, 0, _LCDML_TYPE_dynParam_enableSubMenus);
LCDML_addAdvanced (7 , LCDML_0_7       , 1  , NULL, "", menu_setReference,0,_LCDML_TYPE_dynParam);
LCDML_addAdvanced (8 , LCDML_0_7       , 2  , NULL, "Re-Reference", menu_reReference,0,_LCDML_TYPE_default);
LCDML_addAdvanced (9 , LCDML_0         , 8  , NULL, "Manual Motor", NULL, 0, _LCDML_TYPE_default);
LCDML_addAdvanced (10, LCDML_0_8       , 1  , NULL, "Move Manual", menu_motorMoveManual, 0, _LCDML_TYPE_default);
LCDML_addAdvanced (11, LCDML_0_8       , 2  , NULL, "", menu_motorMoveAngle, 0, _LCDML_TYPE_dynParam);
LCDML_addAdvanced (12, LCDML_0         , 9  , NULL, "Manual Shutter", manualShutter,0,_LCDML_TYPE_default);


// menu element count - last element id
// this value must be the same as the last menu element
#define _LCDML_DISP_cnt    12

// create menu
LCDML_createMenu(_LCDML_DISP_cnt);


//_Internal_functions___________________________________________________________
  long arr2Long(byte *arr) {
    long temp = 0;
    temp += (long)arr[0] << 24;
    temp += (long)arr[1] << 16;
    temp += (long)arr[2] << 8;
    temp += (long)arr[3];
    return temp;
  }
  
  /*Copy any array for saving temporary
    input:  original - byte array to copy
            copy - array reference for copy
            arrayLength - length of arrays 
    Note:   both arrays have to be at least arrayLength entries
              otherwise errors can occur (other values get modified)
            only first arrayLength entries get copied
  */
  void copyArray(byte original[], byte copy[], byte arrayLength) {
    for (int i=0; i < arrayLength; i++) 
      copy[i] = original[i];
  }

  /*insert a specific string in front of a existing string
    input:  str - String to expand
            trailingString - String to add in front of str
            len - desired str length in the end
            trim -  if str should be trimed to len in the end, 
                    usefull if trailingString length is >1
    return: expanded String
  */
  String insertTrailing(String str, 
                        String trailingString,
                        unsigned int len, 
                        bool trim) {
    if (str.length() > len) return str;
    int countTrailing = (len-str.length())/trailingString.length() + 1;
    String temp = "";
    for (int i = 0; i < countTrailing; i++) {
      temp += trailingString;
    }
    temp += str;
    if (trim) {
      temp = temp.substring(temp.length()-len);
    }
    return temp;
  }

  /* converts float angle value to 4-digit String
    input:  value - angle to convert
    return: angle as String
  */
  String angle2String(float value) {
    return insertTrailing(String(value), "0", 5, true);
  }

  String angleArr2String(byte *arr) {
    return angle2String(angleArr2Float(arr));
  }

  float angleArr2Float(byte *arr) {
    return arr[0]*10 + arr[1] + arr[2]*0.1 + arr[3]*0.01;
  }


  /* converts a second value to String with minutes and seconds
    input:  value - in ms
    return: time as String
  */
  String time2String(unsigned long value) {
    unsigned int minute = value/1000/60;
    unsigned int sec = (value/1000) % 60;
    unsigned int millisec = (value % 1000) / 100;
    return insertTrailing(String(minute), "0", 1, true) + ":" 
        + insertTrailing(String(sec), "0", 2, true) + "."
        + String(millisec);
  }

  String timeArr2String(byte *arr) {
    return time2String(((unsigned long)arr[0]*600 
    + (unsigned long)arr[1]*100 
    + (unsigned long)arr[2]*10 
    + (unsigned long)arr[3])*100);
  }

  /* returns corresponding string to enum of wait
    input: enum
    return: String
    */
  String wait2String(unsigned long value) {
    switch(value) {
      case 0: return "False";
      case 1: return "True, exc";
      case 2: return "True, inc";
      default: return "Error";
    }
  }

  String waitArr2String(byte *arr) {
    return wait2String(arr2Long(arr));
  }


  /* returns corresponding string to enum of reference
    input: enum
    return: string
    */
  String reference2String(unsigned long value) {
    switch (value) {
      case 0: return "False";
      case 1: return "True";
      default: return "Error";
    }
  }

  String referenceArr2String(byte *arr) {
    return reference2String(arr2Long(arr));
  }


  /* returns corresponding string to enum of laser
    input: enum
    return: string
    */
  String laser2String(unsigned long value) {
    switch (value) {
      case 0: return "405nm";
      case 1: return "355nm";
      default: return "Error";
    }
  }

  String laserArr2String(byte *arr) {
    return laser2String(arr2Long(arr));
  }


//_Setup________________________________________________________________________
void lcdSetup() {
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, (uint8_t*)scroll_bar[0]);
  lcd.createChar(1, (uint8_t*)scroll_bar[1]);
  lcd.createChar(2, (uint8_t*)scroll_bar[2]);
  lcd.createChar(3, (uint8_t*)scroll_bar[3]);
  lcd.createChar(4, (uint8_t*)scroll_bar[4]);
}

void menuSetup() {
  copyArray(angle, angleTemp, 4);
  copyArray(laser, laserTemp, 4);
  copyArray(time, timeTemp, 4);
  copyArray(wait, waitTemp, 4);
  copyArray(waitTime, waitTimeTemp, 4);
  copyArray(reference, referenceTemp, 4);
  LCDML_setup(_LCDML_DISP_cnt);
  LCDML.MENU_enRollover();
  runner.addTask(checkInputs);
  checkInputs.enable();
}


//_LCDControls__________________________________________________________________
void lcdBacklight(boolean on) {
  if (on) {
    lcd.backlight();
  } else {
    lcd.noBacklight();
  }
}

void lcdClearLine(byte line) {
  String clLine = "                    ";
  lcd.setCursor(0,line);
  lcd.print(clLine);
}

void lcdClear() {
  lcd.clear();
}

void lcdPrint(byte col, byte line, String str) {
  lcd.setCursor(col, line);
  lcd.print(str);
}

//_MenuControls_________________________________________________________________
//times changed to ms
void menu_startExposure(byte param) {
  if (LCDML.FUNC_setup()) {
    float fAngle = angle[0]*10 + angle[1] + angle[2]*0.1 + angle[3]*0.01;
    unsigned long fTime = ((unsigned long)time[0]*600 
                        + (unsigned long)time[1]*100 
                        + (unsigned long)time[2]*10 
                        + (unsigned long)time[3])*100;
    unsigned long fWaitTime = ((unsigned long)waitTime[0]*600 
                        + (unsigned long)waitTime[1]*100 
                        + (unsigned long)waitTime[2]*10 
                        + (unsigned long)waitTime[3])*100; 
    common = startExposure(fAngle, fTime, wait[3], fWaitTime, reference[3]);
    lcdClear();
    if (common == 0) {
      lcdPrint(0,0, "Exposure FINISHED"); 
    } else {
      lcdPrint(0,0,"Exposure ABORTED");
    }
    lcdPrint(0,1, "Angle: " + String(fAngle));
    lcdPrint(0,2, "Duration: " + String((float)fTime/1000) + "s");
    lcdPrint(3,3, "Press OK-Button");
    LCDML.FUNC_setLoopInterval(100);
  }

  if (LCDML.FUNC_loop()) {
    if (LCDML.BT_checkEnter()) {
      LCDML.FUNC_goBackToMenu();
    }
  }

  if (LCDML.FUNC_close()) {
    LCDML.MENU_getCurrentObj();
  }
}


//Change Laser__________________________________________________________________
void menu_setLaser(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if(LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        changeLaserSetting(line);
      }
    }
  }
  String tempString = "Laser:  " + laserArr2String(laser);
  lcd.setCursor(1, line);
  lcd.print(tempString);
}


//Change angle__________________________________________________________________
void menu_setAngle(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if(LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        changeAngleSetting(line);
      }
    }
  }
  String tempString = "";
  if (motorAttached) {
    tempString = "Angle: " + angleArr2String(angle);
  } else  {
    tempString = "Angle: NA";
  }
  lcd.setCursor(1, line);
  lcd.print(tempString);
}


// Change Exposure Time_________________________________________________________
void menu_setTime(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if(LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        changeTimeSetting(line);
      }
    }
  }
  String tempString = "Time:  " + timeArr2String(time);
  lcd.setCursor(1, line);
  lcd.print(tempString);
}


// Change wait settings ________________________________________________________
void menu_setWait(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if(LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        changeWaitSetting(line);
      }
    }
  }
  String tempString = "Wait:  " + waitArr2String(wait);
  lcd.setCursor(1, line);
  lcd.print(tempString);
}


// Change Wait Time_____________________________________________________________
void menu_setWaitTime(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if(LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        changeWaitTimeSetting(line);
      }
    }
  }
  String tempString = "Wait Time: " + timeArr2String(waitTime);
  lcd.setCursor(1, line);
  lcd.print(tempString);
}


// reference menu
void menu_reference(byte line) {
  lcdClearLine(line);
  String tempString = "ReRef: " + referenceArr2String(reference);
  lcd.setCursor(1, line);
  lcd.print(tempString);
}


// Change reference settings ________________________________________________________
void menu_setReference(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if(LCDML.BT_checkAny()) {
      if(LCDML.BT_checkEnter()) {
        changeReferenceSetting(line);
      }
    }
  }
  lcdClearLine(line);
  String tempString = "ReRef: " + referenceArr2String(reference);
  lcd.setCursor(1, line);
  lcd.print(tempString);
}

// re-Reference stage __________________________________________________________
void menu_reReference(byte param){
  if(LCDML.FUNC_setup()) {
    byte errorCode = referenceStage(false, motorReferenceApproach);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Referencing");
    lcd.setCursor(0,1);
    if (errorCode == 0) {
      lcd.print("PASSED");
    } else {
      lcd.print("FAILED");
      lcd.setCursor(0,2);
      String tempString = "Error: " + errorCode;
      lcd.print(tempString);
    }
    lcd.setCursor(0,3);
    lcd.print("Press any button");
  }

  if(LCDML.FUNC_loop()) {
    if (LCDML.BT_checkAny()) {
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close()) {
    LCDML.MENU_getCurrentObj();
  }
}


// Motor move Manual ___________________________________________________________
void menu_motorMoveManual(byte param) {
  if (LCDML.FUNC_setup()) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Manual Motor");
    lcd.setCursor(0,1);
    lcd.print(String("UP: CCW,s ") + VLINE + " R: CCW,f");
    lcd.setCursor(0,2);
    lcd.print(String("DO: CW, s ") + VLINE + " L: CW, f");
    lcd.setCursor(0,3);
    lcd.print("Stop:OK | Quit:Abort");
  }

  if (LCDML.FUNC_loop()) {
    byte tempA[9] = {0,0,0,0,0,0,0,0,0};
    if (LCDML.BT_checkUp()) {
      motorMoveCW(tempA, 1, 6400);
    }
    if (LCDML.BT_checkRight()) {
      motorMoveCW(tempA, 1, 51200);
    }
    if (LCDML.BT_checkDown()) {
      motorMoveCCW(tempA, 1, 6400);
    }
    if (LCDML.BT_checkLeft()) {
      motorMoveCCW(tempA, 1, 51200);
    }
    if (LCDML.BT_checkEnter()) {
      motorStop();
    }
    if (LCDML.BT_checkQuit()) {
      LCDML.FUNC_goBackToMenu();
    }
    LCDML.BT_resetAll();
    runner.execute();
  }

   if(LCDML.FUNC_close()) {
     motorStop();
    LCDML.MENU_getCurrentObj();
  }
}


// Motor Move to angle _________________________________________________________
void menu_motorMoveAngle(byte line) {
  if (line == LCDML.MENU_getCursorPos()) {
    if (LCDML.BT_checkEnter()) {
      manual_MotorAngle(line);
    }
    lcd.cursor_off();
  }
  lcdClearLine(line);
  lcd.setCursor(1, line);
  lcd.print("Move to Angle");
}


// Activate manual shutter______________________________________________________
void manualShutter(byte param){
  if(LCDML.FUNC_setup()) {
    lcdBacklight(true);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Manual Shutter!");
    lcd.setCursor(0,1);
    lcd.print("Interlock Disabled!");
    lcd.setCursor(0,2);
    lcd.print("Wear Eye Protection!");
    lcd.setCursor(0,3);
    lcd.print("Be Careful!");
    detachInterrupt(digitalPinToInterrupt(interlockSystem));
  }

  if(LCDML.FUNC_loop()) {
    Serial.println(interlockStatus);
    interlockStatus = true;
    Serial.println(interlockStatus);
    if (LCDML.BT_checkUp()) {
      interlockStatus = true;
      Serial.println(interlockStatus);
      long tempLaser = arr2Long(laser);
      switch(tempLaser) {
        case 0: shutterOpen(shutter00); break;
        case 1: shutterOpen(shutter01); break;
      }
      LCDML.BT_resetUp();
    }
    if (LCDML.BT_checkDown()) {
      long tempLaser = arr2Long(laser);
      switch(tempLaser) {
        case 0: shutterClose(shutter00); break;
        case 1: shutterClose(shutter01); break;
      }
      LCDML.BT_resetDown();
    }
    if (LCDML.BT_checkAny()) {
      LCDML.FUNC_goBackToMenu();
    }
  }

  if(LCDML.FUNC_close()) {
    attachInterrupt(digitalPinToInterrupt(interlockSystem),isr2, CHANGE);
    long tempLaser = arr2Long(laser);
    switch(tempLaser) {
        case 0: shutterClose(shutter00); break;
        case 1: shutterClose(shutter01); break;
      }
    isr2();
    LCDML.MENU_getCurrentObj();
  }
}

//______________________________________________________________________________
//_Methods copied from example code from libmenu2_______________________________
//______________________________________________________________________________
  void lcdml_menu_control(void) {
    uint8_t buttonValue = checkButtons();
    switch(buttonValue) {
      case 0: {
        LCDML.BT_enter();
        break;
      }
      case 1: {
        LCDML.BT_quit();
        break;
      }
      case 11: {
        LCDML.BT_up();
        break;
      }
      case 12: {
        LCDML.BT_down();
        break;
      }
      case 13: {
        LCDML.BT_left();
        break;
      }
      case 14: {
        LCDML.BT_right();
        break;
      }
    }

    uint8_t serialValue = handleSerial();
    switch(serialValue) {
      case 0: {
        LCDML.BT_enter();
        break;
      }
      case 1: {
        LCDML.BT_quit();
        break;
      }
      case 11: {
        LCDML.BT_up();
        break;
      }
      case 12: {
        LCDML.BT_down();
        break;
      }
      case 13: {
        LCDML.BT_left();
        break;
      }
      case 14: {
        LCDML.BT_right();
        break;
      }
    }
  }


  void lcdml_menu_clear() {
    lcd.clear();
    lcd.setCursor(0, 0);
  }


  void lcdml_menu_display() {
    displayLCD();
  }

  //Method slightly changed to make sure that display is cleared correctly!
  void displayLCD() {
    // update content
    // ***************
    if (LCDML.DISP_checkMenuUpdate()) {
      // clear menu
      // ***************
      LCDML.DISP_clear();
      // declaration of some variables
      // ***************
      // content variable
      char content_text[_LCDML_DISP_cols];  // save the content text of every menu element
      // menu element object
      LCDMenuLib2_menu *tmp;
      LCDMenuLib2_menu *tmp2;
      // some limit values
       

      // check if this element has children
      if ((tmp2 = LCDML.MENU_getDisplayedObj()) != NULL) {
        for (int ket = 0; ket < 2; ket++) {
          LCDML.DISP_clear();
          tmp = tmp2;
          uint8_t i = LCDML.MENU_getScroll();
          uint8_t maxi = (_LCDML_DISP_rows - _LCDML_DSIP_use_header) + i;
          uint8_t n = 0;  
          // loop to display lines
          do {
            // check if a menu element has a condition and if the condition be true
            if (tmp->checkCondition()) {
              // check the type off a menu element
              if(tmp->checkType_menu() == true) {
                // display normal content
                LCDML_getContent(content_text, tmp->getID());
                lcd.setCursor(1, (n+_LCDML_DSIP_use_header));
                lcd.print(content_text);
              } else {
                if(tmp->checkType_dynParam()) {
                  tmp->callback(n);
                }
              }
              // increment some values
              i++;
              n++;
            }
          // try to go to the next sibling and check the number of displayed rows
          } while (((tmp = tmp->getSibling(1)) != NULL) && (i < maxi));
        }
      }
    }

    if(LCDML.DISP_checkMenuCursorUpdate()) {
      // init vars
      uint8_t n_max             = (LCDML.MENU_getChilds() >= (_LCDML_DISP_rows - _LCDML_DSIP_use_header)) ? (_LCDML_DISP_rows - _LCDML_DSIP_use_header) : (LCDML.MENU_getChilds());
      uint8_t scrollbar_min     = 0;
      uint8_t scrollbar_max     = LCDML.MENU_getChilds();
      uint8_t scrollbar_cur_pos = LCDML.MENU_getCursorPosAbs();
      uint8_t scroll_pos        = ((1.*n_max * (_LCDML_DISP_rows - _LCDML_DSIP_use_header)) / (scrollbar_max - 1) * scrollbar_cur_pos);


      // display rows
      for (uint8_t n = 0; n < n_max; n++) {
        //set cursor
        lcd.setCursor(0, (n+_LCDML_DSIP_use_header));

        //set cursor char
        if (n == LCDML.MENU_getCursorPos()) {
          lcd.write(_LCDML_DISP_cfg_cursor);
        } else {
          lcd.write(' ');
        }

        // delete or reset scrollbar
        if (_LCDML_DISP_cfg_scrollbar == 1) {
          if (scrollbar_max > n_max) {
            lcd.setCursor((_LCDML_DISP_cols - 1), (n+_LCDML_DSIP_use_header));
            lcd.write((uint8_t)0);
          }
          else {
            lcd.setCursor((_LCDML_DISP_cols - 1), (n+_LCDML_DSIP_use_header));
            lcd.print(' ');
          }
        }
      }

      // display scrollbar
      if (_LCDML_DISP_cfg_scrollbar == 1) {
        if (scrollbar_max > n_max) {
          //set scroll position
          if (scrollbar_cur_pos == scrollbar_min) {
            // min pos
            lcd.setCursor((_LCDML_DISP_cols - 1), (0+_LCDML_DSIP_use_header));
            lcd.write((uint8_t)1);
          } else if (scrollbar_cur_pos == (scrollbar_max - 1)) {
            // max pos
            lcd.setCursor((_LCDML_DISP_cols - 1), (n_max - 1 + _LCDML_DSIP_use_header));
            lcd.write((uint8_t)4);
          } else {
            // between
            lcd.setCursor((_LCDML_DISP_cols - 1), (scroll_pos / n_max + _LCDML_DSIP_use_header));
            lcd.write((uint8_t)(scroll_pos % n_max) + 1);
          }
        }
      }
    }
  }

//_end methods copied from example code from libmenu2___________________________


//_Start________________________________________________________________________
void startMenu() {
  LCDML.loop();
}
//EOF