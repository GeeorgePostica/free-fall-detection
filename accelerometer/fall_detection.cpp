#include "fall_detection.h"
#include <miosix.h>
#include <pthread.h>
#include <math.h>

// Check the config file for explanation...
#ifndef FD_STOP_ACC_ON_FALL
#define FD_STOP_ACC_ON_FALL
#endif

#define VEC_SQR_MAG(x) x[0]*x[0] + x[1]*x[1] + x[2]*x[2]
#define MAG_SQR_THRES  FD_MAG_THRESHOLD * FD_MAG_THRESHOLD

using namespace miosix;

/*------------ GLOBAL Variables -------------*/
int started = 0;
int crashTestEnabled; // used to check if crash test was enabled

/*       Free Fall Detection related         */
int fallCheckStop; // used to check if fall analysis should stop
int fallCallbackReady; // used to signal when the callback is ready to execute
Timer timer; // calculates the interval when fall is detected
void (*callback)(); // store the callbackFunction parameter

/*          Crash Detection related          */
void (*crashCallback)(); //A function to be called when a free fall crash occurs
impactData impact; // store the fall data (for crash calculation)

pthread_mutex_t fallMutex;

pthread_mutex_t fallCallbackMutex;
pthread_cond_t fallCallbackCond;
/*-------------------------------------------*/

/*------------ Function Prototypes-----------*/
float vectorMag(float xyz[]);
void *vFallCallbackThreadFunc(void *args);
void *vFallChecking(void *args);
int iStopRequested();
int iCrashTest();
void *pvExitFallDetection();

/*-------------------------------------------*/

void vInitFallDetection(void (*callbackFunction)(), void (*crashCallBackFunc)(), int repeated) {
    // Check if the algorithm is already running or no callbackFunction was set
    if (callbackFunction == NULL || started) {
        DEBUG_LOG("Fall detection: FAILED: ");
        started ? DEBUG_LOG("Already running\n") : DEBUG_LOG("No callback\n");
        return;
    }

    started = 1;
    fallCheckStop = 0;
    fallCallbackReady = 0;

    if (crashCallBackFunc != NULL) {
        crashTestEnabled = 1;
        crashCallback = crashCallBackFunc;
    } else {
        crashTestEnabled = 0;
    }

    callback = callbackFunction;
    pthread_t fallCheckThread; // runs the fall and detection algorithm
    pthread_t fallCallbackThread; // runs the free fall callback function

    // Create the fall detection thread
    pthread_create(&fallCheckThread, NULL, vFallChecking, (void*)repeated);

    // Init the free fall callback thread related stuff
    pthread_mutex_init(&fallCallbackMutex, NULL);
    pthread_cond_init(&fallCallbackCond, NULL);
    pthread_create(&fallCallbackThread, NULL, vFallCallbackThreadFunc, (void*)repeated);
}

void *vFallChecking(void *args) {
    // Detach the free fall thread so that it exits once the function terminates
    pthread_detach(pthread_self());

    float accel[3];
    int fd_running;
    int falling; // is set != 0 once a fall was detected

    do {
        // Set the running flag
        fd_running = 1;
        falling = 0;

        /* Initiate the Accelerometer */
        if (!iAccInit()) {
            DEBUG_LOG("Fall detection: FAILED: unable to start accelerometer\n");
            return NULL;
        }

        DEBUG_LOG("Fall detection: ENABLED\n");
        /* Cycle which analyzes the data for potential free fall */
        while (fd_running) {
            vAccGetXYZ(accel, 1);
            DEBUG_FD("stable mag: %f\n", vectorMag(accel));
            if (VEC_SQR_MAG(accel) < MAG_SQR_THRES) { // Potential free fall check
                falling = 1; // Set the falling flag
                fd_running = 0; // and exit the analysis loop
                timer.clear(); // reset the timer
                // Enter a more detailed free fall analysis
                do {
                    timer.start();
                    vAccGetXYZ(accel, FD_AVG_SAMPLES);
                    if (VEC_SQR_MAG(accel) > MAG_SQR_THRES) { //Check if still falling
                        falling = 0; //If not, exit the internal loop
                        fd_running = 1;
                        timer.stop();
                        break;
                    }
                    timer.stop();
                    DEBUG_FD("falling mag: %f\n", vectorMag(accel));
                    // If falling for more than time threshold then free fall was detected
                } while (timer.interval() < FD_TIME_THRESHOLD);

                //If a stop was requested then exit and notify the callback thread..
                if (iStopRequested()) {
                    return pvExitFallDetection();
                }
            }
        }
        // Once here we are sure the device is falling...
        // Notify the thread to launch callback function...
        pthread_mutex_lock(&fallCallbackMutex);
        pthread_cond_signal(&fallCallbackCond);
        pthread_mutex_unlock(&fallCallbackMutex);

        if (crashTestEnabled) { //If crash test was enabled, launch it
            impact.fallDurationMs = timer.interval();
            impact.velocity = ACC_G * timer.interval();

            if (iCrashTest()) {
                return pvExitFallDetection();
            }
        }

    } while ((int) args);   // Repeat if repeated was set

    FD_STOP_ACC_ON_FALL;

    DEBUG_LOG("Fall detection: DISABLED\n");
    return NULL;
}

/**
 * Perform the crash test
 * @return 1 if a stop was requested, 0 if performed normally
 */
int iCrashTest() {
    float xyz[3];
    float magnitude;
    int interval;

    // Clear the timer
    timer.clear();
    interval = 0;

    // Analyze the first set of data
    timer.start();
    vAccGetXYZ(xyz, 1);
    timer.stop();
    magnitude = vectorMag(xyz);
    interval = timer.interval();

    // Analyze data until it is no more in free fall status
    while (magnitude < FD_CRASH_MAG_THRESHOLD) {
        if (iStopRequested()) {
            return 1;
        }
        // Since there is no way to measure velocity, we can only integrate
        impact.velocity += (ACC_G - magnitude)*(timer.interval() - interval);
        interval = timer.interval();
        timer.start();
        vAccGetXYZ(xyz, FD_CRASH_ANALYSIS_SAMPLES);
        timer.stop();
        magnitude = vectorMag(xyz);
    }
    impact.crashOccurred = 1;
    impact.fallDurationMs += interval;
    magnitude = magnitude > ACC_G ? magnitude - ACC_G : ACC_G - magnitude;
    impact.impulse = magnitude * (timer.interval() - interval) - impact.velocity;
    impact.velocity *= 0.001;
    impact.impulse *= 0.001 * FD_CRASH_BOARD_MASS;
    // Make the impulse positive...
    impact.impulse = impact.impulse < 0 ? -impact.impulse : impact.impulse;

    if (!iStopRequested()) {
        crashCallback();
    }
    return iStopRequested();
}

void vStopFallDetection() {
    // Stop the fall detection algorithm
    pthread_mutex_lock(&fallMutex);
    fallCheckStop = 1;
    pthread_mutex_unlock(&fallMutex);

    started = 0;
}

void *vFallCallbackThreadFunc(void *args) {
    // Detach the crash track thread so that it exits once the function terminates
    pthread_detach(pthread_self());

    do {
        /* Wait for the fall detection algorithm to signal that the callback is 
         * ready to go... */
        pthread_mutex_lock(&fallCallbackMutex);

        fallCallbackReady = 1;
        pthread_cond_wait(&fallCallbackCond, &fallCallbackMutex);
        fallCallbackReady = 0;

        pthread_mutex_unlock(&fallCallbackMutex);

        if (iStopRequested()) {
            break;
        }

        callback();

    } while ((int) args && !iStopRequested());
    // Destroy the condition variable and mutex
    pthread_mutex_destroy(&fallCallbackMutex);
    pthread_cond_destroy(&fallCallbackCond);

    return NULL;
}

void *pvExitFallDetection() {
    pthread_mutex_lock(&fallCallbackMutex);
    if (fallCallbackReady) {
        pthread_cond_signal(&fallCallbackCond);
    }
    pthread_mutex_unlock(&fallCallbackMutex);
    pthread_mutex_destroy(&fallMutex);
    return NULL;
}

int iStopRequested() {
    int res;
    pthread_mutex_lock(&fallMutex);
    fallCheckStop ? res = 1 : res = 0;
    pthread_mutex_unlock(&fallMutex);
    return res;
}

impactData xGetImpactData() {
    return impact;
}

float vectorMag(float xyz[]) {
    return sqrtf(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
}