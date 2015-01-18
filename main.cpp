#include <miosix.h>
#include <stdio.h>
#include "accelerometer/acc_controls.h"
#include "accelerometer/acc_registers.h"

using namespace miosix;

typedef Gpio<GPIOD_BASE,14> led1;
typedef Gpio<GPIOD_BASE,15> led2;


int main()
{
    led1::mode(Mode::OUTPUT);
    led2::mode(Mode::OUTPUT);

    led1::low();
    vAccInit();
    //vAccSetRate(ACC_ODR_50Hz);
    //vAccSoftReset();
    for(;;)
    {
        led2::high();
        if(cAccGetINFO(ACC_ADDR_WHO_AM_I)) led1::high();
        printf("Blue LED is ON\n");
        //else led1::low();
        Thread::sleep(2000);
        if(cAccGetINFO(ACC_ADDR_INFO2) == 0) led1::low();
        printf("Blue LED is OFF\n");
        led2::low();
        Thread::sleep(2000);
    }
    
}