#include <miosix.h>
#include <stdio.h>
#include "accelerometer/acc_controls.h"
#include "accelerometer/acc_registers.h"
#include "accelerometer/protocols/spi_acc.h"

using namespace miosix;

typedef Gpio<GPIOD_BASE,14> ledRed_Error;
typedef Gpio<GPIOD_BASE,15> ledBlue_Cycle;
typedef Gpio<GPIOD_BASE,12> ledGreen_OK;


int main()
{
    ledRed_Error::mode(Mode::OUTPUT);
    ledBlue_Cycle::mode(Mode::OUTPUT);
    ledGreen_OK::mode(Mode::OUTPUT);
    
    ledRed_Error::low();
    vAccInit();
    //vAccSetRate(ACC_ODR_50Hz);
    //vAccSoftReset();
    for(;;)
    {
        ledBlue_Cycle::high();
        if(cAccGetINFO(ACC_ADDR_WHO_AM_I) == 0xff){
            ledRed_Error::high();
        }
        else{
            ledGreen_OK::high();
        }
        printf("\nBlue LED is ON\n");
        Thread::sleep(10000);
        if(cAccGetINFO(ACC_ADDR_INFO2) == 0){
            ledRed_Error::low();
        }
        else{
            ledGreen_OK::low();
        }
        printf("\nBlue LED is OFF\n");
        ledBlue_Cycle::low();
        Thread::sleep(10000);
    }
    
}