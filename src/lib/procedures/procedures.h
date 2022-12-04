//Filename: procedures.h

#ifndef PROCEDURES_H
#define PROCEDURES_H

void stopExposure();
byte move2Angle(float angle);
byte startExposure(float degree, unsigned long time, byte wait, unsigned long waitTime, byte reference);
byte referenceStage(boolean repTest, uint16_t approachCount);

#endif
//EOF