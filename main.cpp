#include <miosix.h>
#include <stdio.h>
#include <math.h>
#include "accelerometer/led_alert.h"
#include "accelerometer/accelerometer.h"

//#define NOTIFIER_OFF

using namespace miosix;

int main() {
    vAlertInit();
    vAlertShow(Alert::Loading);
    
    Accelerometer::init();
    delayMs(1000);
    if (Accelerometer::getINFO(ACC_ADDR_WHO_AM_I) != 0x3f) {
        vAlertShow(Alert::Error);
    } else {
        vAlertShow(Alert::Running);
    }
    float xyz[3];
    for (;;) {
        Accelerometer::getXYZ(xyz, 10);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        Thread::sleep(1000);
        Accelerometer::getXYZ(xyz, 10);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        Thread::sleep(1000);
    }

}