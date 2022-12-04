//Filename: menuInterface.h

#include <LCDMenuLib2.h>
#include <LiquidCrystal_I2C.h>
#include <TaskSchedulerDeclarations.h>
#ifndef MENUINTERFACE_H
#define MENUINTERFACE_H

extern LCDMenuLib2 LCDML;
extern Scheduler runner;
extern LiquidCrystal_I2C lcd;


void menu_startExposure(byte param);
void mFunc_screensaver(byte param);
void menu_setLaser(byte param);
void menu_setAngle(byte param);
void menu_setTime(byte param);
void menu_setWait(byte param);
void menu_setWaitTime(byte param);
void menu_reference(byte param);
void menu_reReference(byte param);
void menu_setReference(byte param);
void menu_motorMoveManual(byte param);
void menu_motorMoveAngle(byte param);
void manualShutter(byte param);

boolean ref_0();
boolean ref_1();

long arr2Long(byte *arr);
void copyArray(byte original[], byte copy[], byte arrayLength);
String insertTrailing(String str, 
                        String trailingString,
                        unsigned int len, 
                        bool trim);
String angle2String(float value);
String angleArr2String(byte *arr);
float angleArr2Float(byte *arr);
String time2String(unsigned long value);
String timeArr2String(byte *arr);
String wait2String(unsigned long value);
String waitArr2String(byte *arr);
String referenceArr2String(byte *arr);
String laserArr2String(byte *arr);


void lcdSetup(void);
void menuSetup(void);

void lcdBacklight(boolean on);
void lcdClearLine(byte line);
void lcdClear();
void lcdPrint(byte col, byte line, String str);

void lcdml_menu_control(void);
void lcdml_menu_clear(void);
void lcdml_menu_display(void);

void displayLCD();

void startMenu(void);

#endif
//EOF
