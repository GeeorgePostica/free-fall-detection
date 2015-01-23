#include <miosix.h>
#include <stdio.h>
#include <math.h>
#include "accelerometer/acc_controls.h"

using namespace miosix;

typedef Gpio<GPIOD_BASE, 14> ledRed_Error;
typedef Gpio<GPIOD_BASE, 15> ledBlue_Cycle;
typedef Gpio<GPIOD_BASE, 12> ledGreen_OK;

int main() {
    ledRed_Error::mode(Mode::OUTPUT);
    ledBlue_Cycle::mode(Mode::OUTPUT);
    ledGreen_OK::mode(Mode::OUTPUT);

    ledRed_Error::low();
    vAccInit();
    delayMs(5000);
    if (cAccGetINFO(ACC_ADDR_WHO_AM_I) != 0x3f) {
        ledRed_Error::high();
    } else {
        ledGreen_OK::high();
    }
    float x,y,z;
    float xyz[3];
    for (;;) {
        ledBlue_Cycle::high();
        /*x = fAccGetX(); y = fAccGetY(); z = fAccGetZ();
        printf("(%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                x, y, z, sqrt(x*x + y*y + z*z));*/
        vAccGetAverageXYZ(xyz, 10);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        
        Thread::sleep(1000);
        /*x = fAccGetX(); y = fAccGetY(); z = fAccGetZ();
        printf("(%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                x, y, z, sqrt(x*x + y*y + z*z));*/
        vAccGetAverageXYZ(xyz, 10);
        printf("AVG (%.3f\t%.3f\t%.3f)\t Mag: %.3f\n", 
                xyz[0], xyz[1], xyz[2], 
                sqrt(xyz[0]*xyz[0] + xyz[1]*xyz[1] + xyz[2]*xyz[2]));
        ledBlue_Cycle::low();
        Thread::sleep(1000);
    }

}