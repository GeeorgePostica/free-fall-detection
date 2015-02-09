#include <miosix.h>
#include <stdio.h>
#include <math.h>
#include "accelerometer/acc_controls.h"
#include "accelerometer/led_alert.h"

//#define NOTIFIER_OFF

using namespace miosix;

int main() {
    vAlertInit();
    vAlertShow(Alert::Loading);
    
    vAccInit();
    delayMs(1000);
    if (cAccGetINFO(ACC_ADDR_WHO_AM_I) != 0x3f) {
        vAlertShow(Alert::Error);
    } else {
        vAlertShow(Alert::Running);
    }
    float xyz[3];
    for (;;) {
        vAccGetAverageXYZ(xyz, 10);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        Thread::sleep(1000);
        vAccGetAverageXYZ(xyz, 10);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        Thread::sleep(1000);
    }

}