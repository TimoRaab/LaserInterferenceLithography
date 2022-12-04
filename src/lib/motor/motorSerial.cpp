//Filename: motorSerial.cpp


#include <Arduino.h>   
#include "motorSerial.h"
#include "../procedures/procedures.h"
#include "./lib/buildSettings/compile/compileSettings.h"
#include "./lib/buildSettings/pinConfiguration/pinConfiguration.h"
#include "./lib/buildSettings/setupConfig/setupConfig.h"
   
//_Initialization_______________________________________________________________

void motorSetup() {
  pinMode(rxSoftware, INPUT_PULLUP);
  Serial1.begin(9600);
  pinMode(ctrlPin, OUTPUT);

  // check if motor is attached with 3 calls
  byte countLoop = 0;
  while (countLoop < 3  && motorAttached) {
    byte temp[9] = {0,0,0,0,0,0,0,0,0};
    if (countLoop == 0) {
        motorStop(temp, 1);
    } else {
        motorGetActualPosition(temp, 1);
    }
    countLoop++;
    if (calcChecksum(temp) != temp[8]) {
      motorAttached = false;
    }
  }
  if (motorAttached) referenceStage(false, motorReferenceApproach);
}

//_Methods______________________________________________________________________
  //_Serial_____________________________________________________________________
    /* prints motor respond
       input: temp - n-byte motor respond
              arrayLength - length of array, has to match n-bytes of motor respond
              instruction - instruction send to motor for respond
              type - program specific code, defined in constants-general-program codes
    */
    void printMotorArray(byte temp[], long arrayLength, byte instruction, byte type) {
      Serial.write(instruction);
      Serial.write(type);
      Serial.write(temp, arrayLength);
    }


  //_Preparation_and_Sending____________________________________________________
  //____________________________________________________________________________
    
    /* converts long-value to array (big-endian)
        input: valueArray - 4-byte array for returning value
              value - value for conversion
    */
    void long2Byte(byte valueArray[], long value) {
      valueArray[3] = (byte) value;
      valueArray[2] = (byte) (value >> 8);
      valueArray[1] = (byte) (value >> 16);
      valueArray[0] = (byte) (value >> 24);
    }

    /* calculates checksum for motor 
        input:   inputArray - 9-byte array for calculation
        return:  checksum byte
    */
    byte calcChecksum(byte inputArray[]) {
      byte checksum = 0;
      for (int i=0; i < 8; i++) {
        checksum += inputArray[i];
      }
      return checksum;
    }

    /* prepares array for sending to motor 
        input:   inputArray - 9-byte array (also used for return)
                target - motor number
                instruction - instruction to motor (check documentation)
                type - clarification to instruction (check documentation)
                motorBank -  special instruction clarification (check documentation)
                value -  value for command, transformed to array in method
    */
    void prepareArray4Motor(byte inputArray[], 
                            byte target, 
                            byte instruction,
                            byte type,
                            byte motorBank,
                            long value) {
      byte valueArray[4] = {0,0,0,0};
      if (DEBUG) {
        Serial.print("Prepare: ");
        for (int i=0; i<9; i++) {
          Serial.print(inputArray[i]);
          Serial.print(", ");
        }
        Serial.println("");
      }
      long2Byte(valueArray, value);
      for (int i=4; i <= 7; i++) {
        inputArray[i] = valueArray[i-4];
      }
      inputArray[0] = target;
      inputArray[1] = instruction;
      inputArray[2] = type;
      inputArray[3] = motorBank;
      inputArray[8] = calcChecksum(inputArray);
    }

    /* send instruction to motor
       input:   instruction - instruction to motor (also used for returning reply)
    */
    void motorSendInstruction(byte instruction[]) {
      /*if (DEBUG) {
        Serial.println("Send: ");
        printMotorArray(instruction, 9, instruction[1], REP_OTHER);
      }*/
      digitalWrite(ctrlPin, HIGH);
      Serial1.write(instruction, 9);
      Serial1.flush();

      if (motorAttached) {
        //clear array and prevent correct match
        for (int i=0; i<8; i++) instruction[i] = 0;
        instruction[8] = 1;
        // motor respond
        while (Serial1.available()) Serial1.read();
        digitalWrite(ctrlPin, LOW);
        Serial1.readBytes(instruction, 9);
      }
      /*if (DEBUG) {
        Serial.println("Return: ");
        printMotorArray(instruction, 9, instruction[1], REP_OTHER);
      }*/
    }



  //_Motor_Move_________________________________________________________________
  //____________________________________________________________________________
  
    /* move motor clockwise
       input:   returnArray - 9-byte array for reply
                target - motor number
                motorSpeed - microsteps in seconds
    */
    void motorMoveCW(byte returnArray[], byte target, long motorSpeed) {
      prepareArray4Motor(returnArray, target, MOTOR_MOVECW, 0, 0, motorSpeed);
      motorSendInstruction(returnArray);
    }

    /* move motor counterclockwise
       input:   returnArray - 9-byte array for reply
                target - motor number
                motorSpeed - microsteps in seconds
    */
    void motorMoveCCW(byte returnArray[], byte target, long motorSpeed) {
      prepareArray4Motor(returnArray, target, MOTOR_MOVECCW, 0, 0, motorSpeed);
      motorSendInstruction(returnArray);
    }
    

    /* move motor to specific position
       input:   returnArray - 9-byte array for reply
                target - motor number
                type - movement type (0 absolute, 1 relative, 2 coordinate)
                pos - position to move
    */
    void motorMove2Position(byte returnArray[], byte target, byte type, long pos) {
      prepareArray4Motor(returnArray, target, MOTOR_MOVE2POS, type, 0, pos);
      motorSendInstruction(returnArray);
    }



  //_Motor_Stop_________________________________________________________________
    /* stop motor
       input:   returnArray - 9-byte array for reply
                target - motor number
    */
    void motorStop(byte returnArray[], byte target) {
      prepareArray4Motor(returnArray, target, MOTOR_STOP, 0, 0, 0);
      motorSendInstruction(returnArray);
    }
    
    /* stop motor
       input:   
       Note:    don't return array
                calls universal function with "target byte = 1" 
    */
    void motorStop() {
      byte temp[9] = {0,0,0,0,0,0,0,0,0};
      motorStop(temp, 1);
    }


  //_Motor_Get_Parameter________________________________________________________
    
    /* returns position of motor
       input:   returnArray - 9-byte array for reply
                target - motor number
       return:  motor position
    */
    long motorGetActualPosition(byte returnArray[], byte target) {
      prepareArray4Motor(returnArray, target, MOTOR_GAP, MOTOR_GAP_ACTUALPOS, 0, 0);
      motorSendInstruction(returnArray);
      long pos = 0;
      pos += (long)returnArray[4] << 24;
      pos += (long)returnArray[5] << 16;
      pos += (long)returnArray[6] << 8;
      pos += (long)returnArray[7];
      return pos;
    }
    
    /* returns position of motor
       input:   
       return:  motor position
       Note:    don't return array
                calls universal function with "target byte = 1"
    */
    long motorGetActualPosition() {
      byte temp[9] = {0,0,0,0,0,0,0,0,0};
      return motorGetActualPosition(temp, 1);
    }


  //_Motor_Set_Axis_Parameter__________________________________________________
  /* common function for set axis parameter
    input:  returnArray - 9-byte array for reply
            target - motor number
            type - axis parameter
            value - value to set
  */
  void motorSAP(byte returnArray[], byte target, byte type, long value) {
    prepareArray4Motor(returnArray, target, MOTOR_SAP, type, 0, value);
    motorSendInstruction(returnArray);
  }

  /* sets the current postion as position "value"
    input: value - position to set
    note: should only be used for referencing! Stop motor before
  */
  void motorSetActualPosition(long value) {
    motorStop();
    byte temp[9] = {0,0,0,0,0,0,0,0,0};
    motorSAP(temp, 1, 1, value);
  }
//EOF