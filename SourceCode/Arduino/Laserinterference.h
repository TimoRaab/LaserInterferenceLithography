//Filename: Laserinterference.h

/*
Laserinterference Control Box Program
Usage with Arduino Mega!
Author: Timo Raab
Date: 02.03.2022
Version: 4.1
*/

/*Files and Folders in Program:
    Laserinterference.cpp
    Laserinterference.h
    ./lib
        ./buildSettings
            ./compile
                ./CompileSetting.h
            ./pinConfiguration
                ./pinConfiguration.h
            ./setupConfig
                ./setupConfig.cpp
                ./setupConfig.h
        ./menuInterface
            ./menuFunctions.cpp
            ./menuFunctions.h
            ./menuInterface.cpp
            ./menuInterface.h
        ./motor
            ./motorSerial.cpp
            ./motorSerial.h
        ./procedures
            ./procedures.cpp
            ./procedures.h
        ./servoMotor
            servoMotor.cpp
            servMotoro.h
*/

/*Non standard Libraries:
    Button.h
    Shutter.h
*/







static volatile boolean goInterrupt = false;
static volatile boolean interlockStatus = true;

void isr2();
void interruptFunction();
byte checkButtons();
byte handleSerial();
byte serialCommands();
byte serialCommands(String in);

//EOF