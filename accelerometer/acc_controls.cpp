#include <stdio.h>
#include "acc_controls.h"
#include "protocols/spi_acc.h"

ACC_TypeDef ACC;     /* The data structure which represents the accelerometer */

/************ FUNCTION PROTOTYPES ***********/
short sAccGetAxis(char axisRegAddress);
void vAccSetFIFOMode(const char FIFOMode);
void vAccBlockDataUpdate(int block);
void vAccEnableAddrInc(int enable);
void vAccSetSPIMode(const char SPIMode);

/********************************************/


int iAccInit() {
    vSpiInit();
    char who = cSpiReadByte(ACC_ADDR_WHO_AM_I);
    if (who == ACC_WHO_AM_I) {
        DEBUG_LOG("Initializing %s ...\n", ACC_ID);
        
        /* Soft reset the accelerometer */
        vAccSoftReset();
        
        /* Print the configuration */
        DEBUG_ACC("Configuring the accelerometer: \n%s", ACC_PRINT_INIT_CONFIG);
        
        /* Setting the default values in ACC struct */
        ACC.CTRL_REG3 = ACC_INIT_REG3;
        ACC.CTRL_REG4 = ACC_INIT_REG4;
        ACC.CTRL_REG5 = ACC_INIT_REG5;
        ACC.CTRL_REG6 = ACC_INIT_REG6;
        ACC.FIFO_CTRL = ACC_INIT_FIFO;
        
        /* Writing the registers of accelerometer */
        vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3);
        vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
        vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
        vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
        vSpiWriteByte(ACC_ADDR_FIFO_CTRL, ACC.FIFO_CTRL);
        
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
        DEBUG_LOG("Could not identify %s !!\n", ACC_ID);
    }
    else{
        DEBUG_LOG("Could not connect to %s !!\n", ACC_ID);
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

float fAccGetX() {
    return sAccGetAxis(ACC_ADDR_OUT_X_L) * ACC.scale;
}

float fAccGetY() {
    return sAccGetAxis(ACC_ADDR_OUT_Y_L) * ACC.scale;
}

float fAccGetZ() {
    return sAccGetAxis(ACC_ADDR_OUT_Z_L) * ACC.scale;
}

short sAccGetAxis(char axisRegAddress) {
    short res;
    //vAccBlockDataUpdate(1);
    res = sSpiReadShort(axisRegAddress);
    //vAccBlockDataUpdate(0);
    return res;
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

void vAccEnableAxis(const char Axis, int enable) {
    /* Mask the input (in case a wrong Axis value was passed) */
    enable ? ACC.CTRL_REG4 |= (Axis & ACC_AXIS_MASK)
            : ACC.CTRL_REG4 &= ~(Axis & ACC_AXIS_MASK);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableFIFO(int enable) {
    enable ? ACC.CTRL_REG6 |= ACC_CTRL_REG6_FIFO_ENABLE 
           : ACC.CTRL_REG6 &= ~ACC_CTRL_REG6_FIFO_ENABLE;
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccEnableAddrInc(int enable) {
    enable ? ACC.CTRL_REG6 |= ACC_CTRL_REG6_ADDR_INC 
           : ACC.CTRL_REG6 &= ~ACC_CTRL_REG6_ADDR_INC;
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

int iAccIsDataOverrun() {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & ACC_STATUS_DATA_OVERRUN);
}

int iAccIsDataOverrun(const char Axis) {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & ((Axis & ACC_AXIS_MASK) << 4));
}

int iAccIsDataReady() {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & ACC_STATUS_DATA_READY);
}

int iAccIsDataReady(const char Axis) {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & (Axis & ACC_AXIS_MASK));
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
    
    DEBUG_LOG("Accelerometer %s disconnected\n", ACC_ID);
}