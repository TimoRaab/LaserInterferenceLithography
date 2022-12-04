//Filename: motorSerial.h

#include <Arduino.h>   
#ifndef MOTORSERIAL_H
#define MOTORSERIAL_H


//_movements________________________________________________________________
const byte MOTOR_MOVECCW = 1;   //ROR
const byte MOTOR_MOVECW = 2;    //ROL
const byte MOTOR_STOP = 3;      //MST 
const byte MOTOR_MOVE2POS = 4;  //MVP 
const byte MOTOR_SAP = 5;       //SAP
const byte MOTOR_GAP = 6;       //GAP

//_parameters_______________________________________________________________
const byte MOTOR_MVP_ABSOLUTE = 0;
const byte MOTOR_MVP_RELATIVE = 1;
const byte MOTOR_MVP_COORDINATE = 2;

const byte MOTOR_GAP_TARGETPOS = 0;
const byte MOTOR_GAP_ACTUALPOS = 1;
const byte MOTOR_GAP_TARGETSPEED = 2;
const byte MOTOR_GAP_ACTUALSPEED = 3;

void motorSetup();

//_Serial_______________________________________________________________________
void printMotorArray(byte temp[], long arrayLength, byte instruction, byte type);


//_Preparation_and_Sending______________________________________________________
void long2Byte(byte valueArray[], long value);
byte calcChecksum(byte inputArry[]);
void prepareArray4Motor(byte inputArray[],
                        byte target,
                        byte instruction,
                        byte type,
                        byte motorBank,
                        long value);
void motorSendInstruction(byte instruction[]);


//_Motor_Move___________________________________________________________________
void motorMoveCW(byte returnArray[], byte target, long motorSpeed);

void motorMoveCCW(byte returnArray[], byte target, long motorSpeed);

void motorMove2Position(byte returnArray[], byte target, byte type, long pos);

//_Motor_Stop___________________________________________________________________
void motorStop(byte returnArray[], byte target);
void motorStop();


//_Motor_Get_Parameter__________________________________________________________
long motorGetActualPosition(byte returnArray[], byte target);
long motorGetActualPosition();


//_Motor_Set_Parameter__________________________________________________________
void motorSAP(byte returnArray[], byte target, byte type, long value);
void motorSetActualPosition(long value);

#endif
//EOF