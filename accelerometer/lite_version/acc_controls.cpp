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
        DEBUG_LOG("Initializing accelerometer ...\n");
        
        /* Soft reset the accelerometer */
        vAccSoftReset();
        
        /* Setting the default values in ACC struct */
        ACC.CTRL_REG3 = ACC_INIT_REG3;
        ACC.CTRL_REG4 = ACC_INIT_REG4;
        ACC.CTRL_REG5 = ACC_INIT_REG5;
        ACC.CTRL_REG6 = ACC_INIT_REG6;
        
        /* Writing the registers of accelerometer */
        vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3);
        vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
        vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
        vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
        
        /* Set the full scale */
        vAccSetScale(ACC_CTRL_REG5_FSCALE_2G);
        
        /* Set offsets */
        vAccSetOffsetX(ACC_OFFSET_X);
        vAccSetOffsetY(ACC_OFFSET_Y);
        vAccSetOffsetZ(ACC_OFFSET_Z);
        
        DEBUG_LOG(" ... initialization done\n\n");
        
        /* Return positive outcome */
        return 1;
    }
    else if(who != 00){
        DEBUG_LOG("Could not identify accelerometer!!\n");
    }
    else{
        DEBUG_LOG("Could not connect to accelerometer !!\n");
    }
    return 0;
}

void vAccReboot() {
    DEBUG_LOG("Rebooting accelerometer...\n");
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

void vAccSetScale(const char scale) {
    ACC.CTRL_REG5 = (ACC.CTRL_REG5 & ~ACC_FSCALE_MASK)
            | (scale & ACC_FSCALE_MASK);
    switch (scale) {
        case ACC_CTRL_REG5_FSCALE_4G: 
            ACC.scale = ACC_SCALE_4G;
            ACC.offset = ACC_OFFSET_4G;
            DEBUG_ACC("-> FScale = 4G\n");
            break;
        case ACC_CTRL_REG5_FSCALE_6G: 
            ACC.scale = ACC_SCALE_6G;
            ACC.offset = ACC_OFFSET_6G;
            DEBUG_ACC("-> FScale = 6G\n");
            break;
        case ACC_CTRL_REG5_FSCALE_8G: 
            ACC.scale = ACC_SCALE_8G;
            ACC.offset = ACC_OFFSET_8G;
            DEBUG_ACC("-> FScale = 8G\n");
            break;
        case ACC_CTRL_REG5_FSCALE_16G: 
            ACC.scale = ACC_SCALE_16G;
            ACC.offset = ACC_OFFSET_16G;
            DEBUG_ACC("-> FScale = 16G\n");
            break;
        default: 
            ACC.scale = ACC_SCALE_2G;
            ACC.offset = ACC_OFFSET_2G;
            DEBUG_ACC("-> FScale = 2G\n");
            break;
    }
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
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

void vAccSetRate(const char rate) {
    ACC.CTRL_REG4 = (ACC.CTRL_REG4 & ~ACC_ODR_MASK)
            | (rate & ACC_ODR_MASK);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
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