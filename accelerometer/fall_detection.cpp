#include "fall_detection.h"
#include <miosix.h>
#include <pthread.h>
#include <math.h>

#define VEC_SQR_MAG(x) x[0]*x[0] + x[1]*x[1] + x[2]*x[2]
#define MAG_SQR_THRES  FD_MAG_THRESHOLD * FD_MAG_THRESHOLD

using namespace miosix;

/*------------ GLOBAL Variables -------------*/
int fd_running; // keeps the fall analysis loop going
int falling; // is set != 0 once a fall was detected
Timer *timer; // calculates the interval when fall is detected
void (*callback)(); // store the callbackFunction parameter

int crashCheckRun;
void (*crashCallback)(); //A function to be called when a free fall crash occures
impactData impact; // store the fall data (for crash calculation)
/*-------------------------------------------*/

/*------------ Function Prototypes-----------*/
float vectorMag(float xyz[]);
void *vCrashCheck(void *args);
void *vFallChecking(void *args);

/*-------------------------------------------*/

void vInitFallDetection(void (*callbackFunction)(), void (*crashCallBackFunc)()) {
    // Check if the algorithm is already running or no callbackFunction was set
    if (callbackFunction == NULL || fd_running) {
        DEBUG_LOG("Fall detection: FAILED: ");
        fd_running ? DEBUG_LOG("Already running\n") : DEBUG_LOG("No callback\n");
        return;
    }
    fd_running = 1;
    callback = callbackFunction;
    pthread_t fallCheckThread;
    pthread_t crashCheckThread; // runs the crash analysis algorithm
    pthread_create(&fallCheckThread, NULL, vFallChecking, NULL);
    if (crashCallBackFunc != NULL) {
        crashCallback = crashCallBackFunc;
        pthread_create(&crashCheckThread, NULL, vCrashCheck, NULL);
    }
}

void *vFallChecking(void *args) {
    // Detach the free fall thread so that it exits once the function terminates
    pthread_detach(pthread_self());

    float accel[3];

    timer = new Timer();
    falling = 0;
    
    /* Initiate the Accelerometer */
    if (!iAccInit()) {
        DEBUG_LOG("Fall detection: FAILED: unable to start accelerometer\n");
        return NULL;
    }

    DEBUG_LOG("Fall detection: ENABLED\n");
    /* Cycle which analyzes the data for potential free fall */
    while (fd_running) {
        vAccGetXYZ(accel, FD_AVG_SAMPLES);
        DEBUG_FD("stable mag: %f\n", vectorMag(accel));
        if (VEC_SQR_MAG(accel) < MAG_SQR_THRES) { // Potential free fall check
            // Enter a more detailed free fall analysis
            falling = 1; // Set the falling flag
            fd_running = 0; // and exit the analysis loop
            timer->clear(); // reset the timer
            do {
                timer->start();
                vAccGetXYZ(accel, FD_AVG_SAMPLES);
                if (VEC_SQR_MAG(accel) > MAG_SQR_THRES) { //Check if still falling
                    falling = 0;
                    fd_running = 1;
                    timer->stop();
                    break;
                }
                timer->stop();
                DEBUG_FD("falling mag: %f\n", vectorMag(accel));
            } while (timer->interval() < FD_TIME_THRESHOLD);
            // If falling for more than time threshold then free fall was detected 
        }
    }
    if (falling) {  //If it is falling, invoke the callback function
        if (crashCallback != NULL) {    //If crash test was enabled, launch it
            impact.fallDurationMs = timer->interval();
            impact.velocity = ACC_G * timer->interval();
            DEBUG_LOG("Launching CRASH TEST...\n");
            crashCheckRun = 1;
        }
        callback(); // call the callback function
    }
    if(!crashCheckRun){
        vAccStop();
    }
    DEBUG_LOG("Fall detection: DISABLED\n");
    return NULL;
}

void vStopFallDetection() {
    fd_running = 0;
    crashCheckRun = 0;
}

void *vCrashCheck(void *args) {
    // Detach the crash track thread so that it exits once the function terminates
    pthread_detach(pthread_self());

    Timer* timer = new Timer();
    float xyz[3];
    float magnitude;
    int interval;
    while (crashCheckRun == 0){
        delayMs(1);
    }
    timer->start();
    vAccGetXYZ(xyz, FD_CRASH_ANALYSIS_SAMPLES);
    timer->stop();
    magnitude = vectorMag(xyz);
    interval = timer->interval();
    while (magnitude < FD_MAG_THRESHOLD && crashCheckRun) {
        impact.velocity += (ACC_G - magnitude)*(timer->interval() - interval);
        interval = timer->interval();
        timer->start();
        vAccGetXYZ(xyz, FD_CRASH_ANALYSIS_SAMPLES);
        timer->stop();
        magnitude = vectorMag(xyz);
    }
    if (!crashCheckRun) {
        return NULL;
    }
    impact.crashOccurred = 1;
    impact.fallDurationMs += interval;
    impact.impulse = (ACC_G - magnitude)*(timer->interval() - interval) - impact.velocity;
    impact.velocity *= 0.001;
    impact.impulse *= 0.001;
    crashCallback();

    vAccStop();
    return NULL;
}

impactData xGetImpactData() {
    return impact;
}

float vectorMag(float xyz[]) {
    return sqrtf(xyz[0] * xyz[0] + xyz[1] * xyz[1] + xyz[2] * xyz[2]);
}