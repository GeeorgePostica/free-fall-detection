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
int fallCheckStop;          // used to check if fall analysis should stop
Timer timer; // calculates the interval when fall is detected
void (*callback)(); // store the callbackFunction parameter

/*          Crash Detection related          */
int crashCheckStop;
void (*crashCallback)(); //A function to be called when a free fall crash occurs
impactData impact; // store the fall data (for crash calculation)

pthread_mutex_t crashMutex;
pthread_cond_t crashCond;
/*-------------------------------------------*/

/*------------ Function Prototypes-----------*/
float vectorMag(float xyz[]);
void *vCrashCheck(void *args);
void *vFallChecking(void *args);

/*-------------------------------------------*/

void vInitFallDetection(void (*callbackFunction)(), void (*crashCallBackFunc)()) {
    // Check if the algorithm is already running or no callbackFunction was set
    if (callbackFunction == NULL || started) {
        DEBUG_LOG("Fall detection: FAILED: ");
        started ? DEBUG_LOG("Already running\n") : DEBUG_LOG("No callback\n");
        return;
    }
    
    started = 1;
    
    if (crashCallBackFunc != NULL) {
        crashTestEnabled = 1;
        crashCallback = crashCallBackFunc;
    } else {
        crashTestEnabled = 0;
    }
    
    callback = callbackFunction;
    pthread_t fallCheckThread; // runs the fall detection algorithm
    pthread_t crashCheckThread; // runs the crash analysis algorithm

    // Create the fall detection thread
    pthread_create(&fallCheckThread, NULL, vFallChecking, NULL);
    
    // Check if the crash detection algorithm should be launched
    if (crashTestEnabled) {
        // Initialize the stuff related to crash detection thread
        pthread_mutex_init(&crashMutex, NULL);
        pthread_cond_init(&crashCond, NULL);
        pthread_create(&crashCheckThread, NULL, vCrashCheck, NULL);
    }
}

void *vFallChecking(void *args) {
    // Detach the free fall thread so that it exits once the function terminates
    pthread_detach(pthread_self());
    
    float accel[3];
    int fd_running;
    int falling; // is set != 0 once a fall was detected
    
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
            if(fallCheckStop){
                break;
            }
        }
    }
    if (falling) { //If it is falling, invoke the callback function
        if (crashTestEnabled) { //If crash test was enabled, launch it
            impact.fallDurationMs = timer.interval();
            impact.velocity = ACC_G * timer.interval();

            //Launching CRASH TEST...
            pthread_mutex_lock(&crashMutex);
            pthread_cond_signal(&crashCond);
            pthread_mutex_unlock(&crashMutex);
        }

        if(!fallCheckStop){
            callback(); // call the callback function
        }
    }
    
    // If crash test was not launched, then stop the accelerometer, 
    // otherwise let the crash test to stop it
    if (!crashTestEnabled) {
        FD_STOP_ACC_ON_FALL;
    }
    
    DEBUG_LOG("Fall detection: DISABLED\n");
    return NULL;
}

void vStopFallDetection() {
    // Stop the fall detection algorithm
    fallCheckStop = 1;
    
    // Stop the crash detection algorithm
    crashCheckStop = 1;
    
    started = 0;
}

void *vCrashCheck(void *args) {
    // Detach the crash track thread so that it exits once the function terminates
    pthread_detach(pthread_self());
    
    float xyz[3];
    float magnitude;
    int interval;

    /* Wait for the fall detection algorithm to signal the crash test start
     * this is made to avoid thread creation overhead, that is, create the thread
     * during fall detection initialization and let it wait for a certain signal */
    pthread_mutex_lock(&crashMutex);
    pthread_cond_wait(&crashCond, &crashMutex);
    pthread_mutex_unlock(&crashMutex);

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
    while (magnitude < FD_CRASH_MAG_THRESHOLD && !crashCheckStop) {
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

    if(!crashCheckStop){
        crashCallback();                
    }
    
    FD_STOP_ACC_ON_FALL;
    
    // Destroy the condition variable and mutex
    pthread_mutex_destroy(&crashMutex);
    pthread_cond_destroy(&crashCond);

    return NULL;
}

impactData xGetImpactData() {
    return impact;
}

float vectorMag(float xyz[]) {
    return sqrtf(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
}