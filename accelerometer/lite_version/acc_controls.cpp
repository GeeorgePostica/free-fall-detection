#include <stdio.h>
#include "acc_controls.h"
#include "protocols/spi_acc.h"

ACC_TypeDef ACC;     /* The data structure which represents the accelerometer */

/************ FUNCTION PROTOTYPES ***********/
void vAccBlockDataUpdate(int block);
/********************************************/


int iAccInit() {
    vSpiInit();
    char who = cSpiReadByte(ACC_ADDR_WHO_AM_I);
    if (who == ACC_WHO_AM_I) {
        /* Setting the default values in ACC struct */
        ACC.CTRL_REG3 = ACC_INIT_REG3;
        ACC.CTRL_REG4 = ACC_INIT_REG4;
        ACC.CTRL_REG5 = ACC_INIT_REG5;
        ACC.CTRL_REG6 = ACC_INIT_REG6;
        
        ACC.scale = ACC_SCALE_2G;
        ACC.offset = ACC_OFFSET_2G;    
        
        /* Writing the registers of accelerometer */
        vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3);
        vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
        vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
        vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
        
        /* Set offsets */
        vAccSetOffsetX(ACC_OFFSET_X);
        vAccSetOffsetY(ACC_OFFSET_Y);
        vAccSetOffsetZ(ACC_OFFSET_Z);
        
        /* Return positive outcome */
        return 1;
    }
    return 0;
}

void vAccReboot() {
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6 | ACC_CTRL_REG6_REBOOT);
    iAccInit();
}

void vAccSoftReset() {
    vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3 | ACC_CTRL_REG3_SOFT_RESET);
}

void vAccGetXYZ(float xyz[], int samples){
    if(samples < 1){
        return;
    }
    short values[3];
    int i=0;
    while(i < samples){
        if(iAccIsDataReady()) {
            vAccBlockDataUpdate(1);
            vSpiReadArrayShort(ACC_ADDR_OUT_X_L, values, 3);
            xyz[0] += (float) values[0];
            xyz[1] += (float) values[1];
            xyz[2] += (float) values[2];
            i++;
        }
    }
    xyz[0] *= ACC.scale / (float)samples;
    xyz[1] *= ACC.scale / (float)samples;
    xyz[2] *= ACC.scale / (float)samples;
}

void vAccSetOffsetX(char offset){
    vSpiWriteByte(ACC_ADDR_OFF_X, offset/ACC.offset);
}

void vAccSetOffsetY(char offset){
    vSpiWriteByte(ACC_ADDR_OFF_Y, offset/ACC.offset);
}

void vAccSetOffsetZ(char offset){
    vSpiWriteByte(ACC_ADDR_OFF_Z, offset/ACC.offset);
}

void vAccBlockDataUpdate(int block) {
    block ? ACC.CTRL_REG4 |= ACC_CTRL_REG4_BLOCK_DATA_UPDATE 
          : ACC.CTRL_REG4 &= ~ACC_CTRL_REG4_BLOCK_DATA_UPDATE;
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

int iAccIsDataReady() {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & ACC_STATUS_DATA_READY);
}

char cAccGetINFO(char infoReg) {
    return cSpiReadByte(infoReg);
}

void vAccStop(){
    // Power down the accelerometer
    ACC.CTRL_REG4 |= ACC_CTRL_REG4_ODR_POWER_DOWN;
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
    
    // Stop the SPI
    vSpiShutdown();
    
    DEBUG_LOG("Accelerometer disconnected\n");
}