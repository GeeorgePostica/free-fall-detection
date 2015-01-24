#include <miosix.h>
#include <stdio.h>
#include <math.h>
#include "accelerometer/acc_controls.h"
#include "accelerometer/led_alert.h"

//#define NOTIFIER_OFF

using namespace miosix;

#ifdef NOTIFIER_OFF
typedef Gpio<GPIOD_BASE, 14> ledRed_Error;
typedef Gpio<GPIOD_BASE, 15> ledBlue_Cycle;
typedef Gpio<GPIOD_BASE, 12> ledGreen_OK;
#endif

int main() {
#ifdef NOTIFIER_OFF
    ledRed_Error::mode(Mode::OUTPUT);
    ledBlue_Cycle::mode(Mode::OUTPUT);
    ledGreen_OK::mode(Mode::OUTPUT);

    ledRed_Error::low();
#else
    vAlertInit();
    vAlertShow(Alert::Loading);
#endif
    vAccInit();
    delayMs(2000);
#ifdef NOTIFIER_OFF
    if (cAccGetINFO(ACC_ADDR_WHO_AM_I) != 0x3f) {
        ledRed_Error::high();
    } else {
        ledGreen_OK::high();
    }
    float xyz[3];
#endif
    for (;;) {
#ifdef NOTIFIER_OFF
        ledBlue_Cycle::high();
        vAccGetAverageXYZ(xyz, 30);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        
        Thread::sleep(1000);
        vAccGetAverageXYZ(xyz, 30);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        ledBlue_Cycle::low();
        Thread::sleep(1000);
#else
        vAlertShow(Alert::Falling);
        Thread::sleep(8000);
        vAlertShow(Alert::Running);
        Thread::sleep(5000);
#endif
    }

}