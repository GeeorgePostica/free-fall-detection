#include <miosix.h>
#include <stdio.h>
#include <math.h>
#include "accelerometer/config.h"
#include "accelerometer/fall_detection.h"
#include "accelerometer/led_alert.h"

//#define NOTIFIER_OFF

using namespace miosix;

void falling() {
    vAlertShow(Alert::Falling);
    DEBUG_LOG("\n**** ALERT! FREE FALL DETECTED ****\n");
}

void crashing() {
    if (xGetImpactData().velocity > 1.5) {
        vAlertShow(Alert::Crash);
        DEBUG_LOG("CRASH impact velocity: %f m/s\n", xGetImpactData().velocity);
        DEBUG_LOG("CRASH impact impulse: %f N*s\n", xGetImpactData().impulse);
        DEBUG_LOG("CRASH fall duration: %d ms\n", xGetImpactData().fallDurationMs);
    }
    // Just to reset the LEDs
    Thread::sleep(4000);
    vAlertShow(Alert::Running);
}

int main() {
    vAlertInit();
    vAlertShow(Alert::Loading);
    vInitFallDetection(falling, crashing, 1);       //Repeated
    vAlertShow(Alert::Running);
    for (;;) {

        // Any code can be put here, the fall detection is running in parallel

    }
}