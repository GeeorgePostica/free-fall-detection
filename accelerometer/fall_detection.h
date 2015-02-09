/* 
 * File:   fall_detection.h
 * Author: george
 *
 * Created on January 23, 2015, 8:05 PM
 */

#ifndef FALL_DETECTION_H
#define	FALL_DETECTION_H

#include "acc_controls.h"

/** Number of samples of accelerometer data to be averaged */
#define FD_AVG_SAMPLES      10

/** Acceleration vector magnitude threshold, which tells whether
 *  there is a free fall or not. */
#define FD_MAG_THRESHOLD    2.0

/** Amount of time in ms that the accelerometer vector magnitude should be
 *  below threshold to be considered a free fall */
#define FD_TIME_THRESHOLD   10

/** Default HW Timer for free fall counting */
#define FD_TIMER            TIM2

/** Timer tick divider to get us */
#define FD_TICK_TO_us       168

/** Configures and starts a new thread which checks for a fall detection
 * and when a fall is detected a specified callback function of type void
 *  is invoked */
void vInitFallDetection(void (*callbackFunction)());

/** Stops the fall detection algorithm along with every other communication
 * with the accelerometer. <b> It is mandatory to call this function before
 * exiting the caller function, otherwise it will keep the thread alive</b> */
void vStopFallDetection();

/***************** FOR TESTING PURPOSES ONLY ***********/
typedef struct{
    int fallDurationMs;
    double impactVelocity;
    double impactImpulse;
};
int iCrashOccured();
/*******************************************************/

#endif	/* FALL_DETECTION_H */

