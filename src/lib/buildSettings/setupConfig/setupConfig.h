//Filename: setupConfig.h

#ifndef SETUPCONFIG_H
#define SETUPCONFIG_H

extern byte angle[4];
extern byte laser[4];
extern byte time[4]; //= {0,0,1,0};
extern byte wait[4]; //= {0,0,0,1};
extern byte waitTime[4]; //= {0,0,0,5};
extern byte reference[4]; //= {0,0,0,0}

extern byte angleTemp[4];
extern byte laserTemp[4];
extern byte timeTemp[4];
extern byte waitTemp[4];
extern byte waitTimeTemp[4]; 
extern byte referenceTemp[4];

extern byte maxAngle[4];
extern byte maxLaser[4];
extern byte maxTime[4];
extern byte maxWait[4];
extern byte maxWaitTime[4];
extern byte maxReference[4];

extern byte standardAngle[4];
extern byte standardLaser[4];
extern byte standardTime[4];
extern byte standardWait[4];
extern byte standardWaitTime[4];
extern byte standardReference[4];

extern byte anglePositions[4];
extern byte laserPositions[4];
extern byte timePositions[4];
extern byte waitPositions[4];
extern byte waitTimePositions[4];
extern byte referencePositions[4];


extern boolean motorAttached;
extern long motorZero;

extern uint16_t motorReferenceApproach;

#define waitFalse 0
#define waitExc 1
#define waitInc 2

#define refFalse 0
#define refTrue 1

#define VLINE "\x7C"


//for motor reproducibility test
//can be expanded if needed
#define REP_REFERENCE  0
#define REP_EXPOSURE  1
#define REP_OTHER 255 // use for anything other than them above

void setupConfigSetup();

#endif
//EOF