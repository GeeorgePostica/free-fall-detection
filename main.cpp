#include <miosix.h>
#include "accelerometer/acc_controls.h"

using namespace miosix;

int main()
{
    vAccInit();
    for(;;)
    {
        ledOn();
        Thread::sleep(1000);
        ledOff();
        Thread::sleep(1000);
    }
}