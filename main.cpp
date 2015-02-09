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

int main() {
    vAlertInit();
    vAlertShow(Alert::Loading);
    vInitFallDetection(falling);
    vAlertShow(Alert::Running);
    for (;;) {
        DEBUG_LOG("Tick\n");
        Thread::sleep(10000);
    }

}