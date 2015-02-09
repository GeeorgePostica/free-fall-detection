#include "fall_detection.h"
#include "led_alert.h"
#include <pthread.h>
#include <math.h>
#include <miosix.h>

using namespace miosix;

int stable;
int falling;
Timer *timer;

void vInitFallDetection(void (*callbackFunction)()){
    timer = new Timer();
    Timer *timerTMP;
    
    falling = 0;
    stable = 1;
    
    vAccInit();
    
    while(stable){
        
    }
}


void vStopFallDetection(){
    
}