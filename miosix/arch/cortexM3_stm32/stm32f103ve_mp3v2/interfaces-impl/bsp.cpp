/***************************************************************************
 *   Copyright (C) 2010 by Terraneo Federico                               *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   As a special exception, if other files instantiate templates or use   *
 *   macros or inline functions from this file, or you compile this file   *
 *   and link it with other works to produce a work based on this file,    *
 *   this file does not by itself cause the resulting work to be covered   *
 *   by the GNU General Public License. However the source code for this   *
 *   file must still be made available in accordance with the GNU General  *
 *   Public License. This exception does not invalidate any other reasons  *
 *   why a work based on this file might be covered by the GNU General     *
 *   Public License.                                                       *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, see <http://www.gnu.org/licenses/>   *
 ***************************************************************************/ 

/***********************************************************************
* bsp.cpp Part of the Miosix Embedded OS.
* Board support package, this file initializes hardware.
************************************************************************/


#include <cstdlib>
#include <inttypes.h>
#include "interfaces/bsp.h"
#ifdef WITH_FILESYSTEM
#include "kernel/filesystem/filesystem.h"
#endif //WITH_FILESYSTEM
#include "kernel/kernel.h"
#include "kernel/sync.h"
#include "interfaces/delays.h"
#include "interfaces/portability.h"
#include "interfaces/arch_registers.h"
#include "config/miosix_settings.h"
#include "kernel/logging.h"
#include "drivers/serial.h"

namespace miosix {

//
// Initialization
//

void IRQbspInit()
{
    //On this board this part of the initialization is done by the bootloader
    ledOn();
    delayMs(100);
    ledOff();
    miosix::IRQserialInit();
}

void bspInit2()
{
    //Nothing to do
}

//
// Shutdown and reboot
//

/**
This function disables filesystem (if enabled), serial port (if enabled) and
puts the processor in deep sleep mode.<br>
Wakeup occurs when PA.0 goes high, but instead of sleep(), a new boot happens.
<br>This function does not return.<br>
WARNING: close all files before using this function, since it unmounts the
filesystem.<br>
When in shutdown mode, power consumption of the miosix board is reduced to ~
5uA??, however, true power consumption depends on what is connected to the GPIO
pins. The user is responsible to put the devices connected to the GPIO pin in the
minimal power consumption mode before calling shutdown(). Please note that to
minimize power consumption all unused GPIO must not be left floating.
*/
void shutdown()
{
    pauseKernel();
    #ifdef WITH_FILESYSTEM
    Filesystem& fs=Filesystem::instance();
    if(fs.isMounted()) fs.umount();
    #endif //WITH_FILESYSTEM
    //Disable interrupts
    disableInterrupts();
    if(IRQisSerialEnabled()) IRQserialDisable();

    NVIC_SystemReset();
	for(;;) ; //Never reach here
}

void reboot()
{
    while(!serialTxComplete()) ;
    pauseKernel();
    //Turn off drivers
    #ifdef WITH_FILESYSTEM
    Filesystem::instance().umount();
    #endif //WITH_FILESYSTEM
    disableInterrupts();
    if(IRQisSerialEnabled()) IRQserialDisable();
    miosix_private::IRQsystemReboot();
}

};//namespace miosix
