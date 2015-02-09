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
}

int main() {
    vAlertInit();
    vAlertShow(Alert::Loading);
    vInitFallDetection(falling);
    for (;;) {
        DEBUG_LOG("\nTick");
        Thread::sleep(10000);
    }

}