#include <miosix.h>
#include "accelerometer/acc_registers.h"

using namespace miosix;

int main()
{
    for(;;)
    {
        ledOn();
        Thread::sleep(1000);
        ledOff();
        Thread::sleep(1000);
    }
}