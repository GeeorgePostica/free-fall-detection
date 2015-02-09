#include "fall_detection.h"
#include "led_alert.h"
#include <pthread.h>
#include <math.h>
#include <miosix.h>

using namespace miosix;

/*------------ GLOBAL Variables -------------*/
int stable;         // keeps the fall analysis loop going
int falling;        // is set != 0 once a fall was detected
pthread_t crashCheckThread; // runs the crash analysis algorithm
Timer *timer;               // calculates the interval when fall is detected
void (*callback)();         // store the callbackFunction parameter
/*-------------------------------------------*/

/*------------ Function Prototypes-----------*/
float vectorMag(float xyz[]);
void *vCrashCheck(void *args);
void *vFallChecking(void *args);
/*-------------------------------------------*/

void vInitFallDetection(void (*callbackFunction)()){
    callback = callbackFunction;
    pthread_t fallCheckThread;
    pthread_create(&fallCheckThread, NULL, vFallChecking, NULL);
}

void *vFallChecking(void *args){
    // Detach the free fall thread so that it exits once the function terminates
    pthread_detach(pthread_self());
    
    int time;
    float xyz[3];
    
    timer = new Timer();
    falling = 0;
    stable = 1;
    
    /* Initiate the Accelerometer */
    vAccInit();
    
    DEBUG_LOG("\nFall detection: ENABLED\n");
    /* Cycle which analyzes the data for potential free fall */
    while(stable){
        time = 0;
        vAccGetXYZ(xyz, FD_AVG_SAMPLES);
        if(vectorMag(xyz) < FD_MAG_THRESHOLD){ // Potential free fall check
            // Enter a more detailed free fall analysis
            do{
                timer->start();
                vAccGetXYZ(xyz, FD_AVG_SAMPLES);
                if(vectorMag(xyz) > FD_MAG_THRESHOLD){ //Check if still falling
                    break;
                }
                timer->stop();
                time = timer->interval();
            }while(time/FD_TICK_TO_ms < FD_TIME_THRESHOLD);
            // If falling for more than time threshold then free fall was detected
            falling = 1;    // Set the falling flag
            stable = 0;     // and exit the analysis loop
        }
    }
    if(falling){
        pthread_create(&crashCheckThread, NULL, vCrashCheck, NULL);
        callback();         // call the callback function
    }
    DEBUG_LOG("\nFall detection: DISABLED\n");
    return NULL;
}

void vStopFallDetection(){
    if(!falling){
        stable = 0;
    }
    else{
        pthread_join(crashCheckThread, NULL);
    }
}

void *vCrashCheck(void *args){
    // TODO: Code to fill in the crash data
    
    return NULL;
}

float vectorMag(float xyz[]){
    return sqrtf(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]);
}