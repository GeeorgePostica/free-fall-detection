#include <miosix.h>
#include "acc_controls.h"
#include "acc_registers.h"
#include "protocols/spi_acc.h"

void vAccInit(){
    vAccRegSetSPIMode(ACC_SPI_MODE_4WIRE);
    
}

void vAccReboot(){
    vAccRegForceReboot();
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccSoftReset(){
    vAccRegSoftReset();
    vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3);
}

float fAccGetX(){
    
}

float fAccGetY(){
    
}

float fAccGetZ(){
    
}

float* pfAccGetXYZ(){
    
}
