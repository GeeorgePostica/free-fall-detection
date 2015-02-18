#include <miosix.h>
#include <stdio.h>
#include <math.h>
#include "accelerometer/config.h"
#include "accelerometer/fall_detection.h"
#include "accelerometer/led_alert.h"

//#define NOTIFIER_OFF

using namespace miosix;

void falling(){
    vAlertShow(Alert::Falling);
    DEBUG_LOG("\n**** ALERT! FREE FALL DETECTED ****\n");
}

void crashing(){
    DEBUG_LOG("CRASH impact velocity: %f m/s\n", xGetImpactData().velocity);
    DEBUG_LOG("CRASH fall duration: %d ms\n", xGetImpactData().fallDurationMs);
    
    if(xGetImpactData().velocity > 0.01){
        vAlertShow(Alert::Crash);
    }
}

int main() {
    vAlertInit();
    vAlertShow(Alert::Loading);
    vInitFallDetection(falling, crashing);
    vAlertShow(Alert::Running);
    Timer* tim = new Timer();
    for (;;) {
        DEBUG_LOG("Tick\n");
        tim->start();
        Thread::sleep(10000);
        tim->stop();
        DEBUG_LOG("Timer: %d\n", tim->interval());
    }
}