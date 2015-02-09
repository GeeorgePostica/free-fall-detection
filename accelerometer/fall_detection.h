/* 
 * File:   fall_detection.h
 * Author: george
 *
 * Created on January 23, 2015, 8:05 PM
 */

#ifndef FALL_DETECTION_H
#define	FALL_DETECTION_H

#include "config.h"
#include "acc_controls.h"

/** Configures and starts a new thread which checks for a fall detection
 * and when a fall is detected a specified callback function of type void
 *  is invoked */
void vInitFallDetection(void (*callbackFunction)());

/** Stops the fall detection algorithm along with every other communication
 * with the accelerometer. <b> It is mandatory to call this function before
 * exiting the caller function, otherwise it will keep the thread alive</b> */
void vStopFallDetection();

/***************** FOR TESTING PURPOSES ONLY ***********/
typedef struct fallData{
    int crashOccurred;
    int fallDurationMs;
    double impactVelocity;
    double impactImpulse;
} fallData;
int iCrashOccured();
/*******************************************************/

#endif	/* FALL_DETECTION_H */

