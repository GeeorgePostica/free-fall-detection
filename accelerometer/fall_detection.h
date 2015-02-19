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
 *  is invoked
 * @param callbackFunction the function of type void() to be called when a 
 * free fall is detected, <i>Note</i>: if it is NULL the algorithm won't start
 * @param crashCallBackFunc the function of type void() to be called when 
 * a crash from free fall is detected, 
 * <i>Note</i>: if set NULL then the crash check algorithm won't be started
 * @param repeated set to 1 if the fall-detection should continue working until
 * a Stop is requested, 0 if only one shot fall detection */
void vInitFallDetection(void (*callbackFunction)(), void (*crashCallBackFunc)(), int repeated);

/** Stops the fall detection algorithm along with every other communication
 * with the accelerometer. <b> It is mandatory to call this function before
 * exiting the caller function, otherwise it will keep the thread alive</b> */
void vStopFallDetection();


/***************** FOR TESTING PURPOSES ONLY ***********/
typedef struct impactData{
    int crashOccurred;
    int fallDurationMs;
    float velocity;
    float impulse;
} impactData;
/*******************************************************/

/** Gets the impact data if there was an impact from a free fall */
impactData xGetImpactData();

#endif	/* FALL_DETECTION_H */

