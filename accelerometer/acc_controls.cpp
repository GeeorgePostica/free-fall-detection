#include <stdio.h>
#include "acc_controls.h"
#include "protocols/spi_acc.h"

ACC_TypeDef ACC;        /* The data structure which represents the accelerometer */

/************ FUNCTION PROTOTYPES ***********/
short sAccGetAxis(char axisRegAddress);
void vAccSetFIFOMode(const char FIFOMode);
void vAccBlockDataUpdate(int block);
void vAccEnableAddrInc(int enable);
void vAccSetSPIMode(const char SPIMode);

/********************************************/


void vAccInit() {
    vSpiInit();
    char who = cSpiReadByte(ACC_ADDR_WHO_AM_I);
    if (who == ACC_WHO_AM_I) {
        DEBUG_LOG("\nInitializing LIS3DSH ...\n");
        
        /* Soft reset the accelerometer */
        vAccSoftReset();
        
        /* Print the configuration */
        DEBUG_ACC("Configuring the accelerometer: \n");
        DEBUG_ACC("-> Data ready disabled, interrupts disabled\n");
        DEBUG_ACC("-> 800Hz ODR, x/y/z enabled\n");
        DEBUG_ACC("-> 800Hz AA, no self-test, 4-wire SPI\n");
        DEBUG_ACC("-> FIFO disabled, Auto-increment enabled\n");
        DEBUG_ACC("-> FIFO turned off\n");
        
        /* Setting the default values in ACC struct */
        ACC.CTRL_REG3 = 0x00;  // Data ready disabled, interrupts disabled
        ACC.CTRL_REG4 = 0x87;  // 800Hz ODR, x/y/z enabled
        ACC.CTRL_REG5 = 0x00;  // 800Hz AA, no self-test, 4-wire SPI
        ACC.CTRL_REG6 = 0x10;  // FIFO disabled, Auto-increment enabled
        ACC.FIFO_CTRL = 0x00;  // FIFO turned off
        
        /* Writing the registers of accelerometer */
        vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3);
        vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
        vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
        vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
        vSpiWriteByte(ACC_ADDR_FIFO_CTRL, ACC.FIFO_CTRL);
        
        /* Set the full scale */
        vAccSetScale(ACC_CTRL_REG5_FSCALE_2G);
        
        
        /* Set offsets */
        //vAccSetOffsetX(ACC_OFFSET_X);
        //vAccSetOffsetY(ACC_OFFSET_Y);
        vAccSetOffsetZ(ACC_OFFSET_Z);
        
        DEBUG_LOG(" ... initialization done\n\n");
    }
    else if(who != 00){
        DEBUG_LOG("\nCould not identify LIS3DSH !!\n");
    }
    else{
        DEBUG_LOG("\nCould not connect to LIS3DSH !!\n");
    }
}

void vAccReboot() {
    DEBUG_LOG("\n\nRebooting accelerometer...\n");
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6 | (1<<7));
    vAccInit();
}

void vAccSoftReset() {
    vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3 | 1);
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

void vAccGetXYZ(float xyz[]) {
    short values[3];
    vAccBlockDataUpdate(1);
    vSpiReadArrayShort(ACC_ADDR_OUT_X_L, values, 3);
    //vAccBlockDataUpdate(0);
    /*Order reversed due to SPI protocol*/
    xyz[0] = values[0] * ACC.scale;
    xyz[1] = values[1] * ACC.scale;
    xyz[2] = values[2] * ACC.scale;
}

void vAccGetAverageXYZ(float xyz[], int samples){
    if(samples < 1){
        return;
    }
    short values[3];
    int i=0;
    while(i < samples){
        if(iAccIsDataReady()) {
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

void vAccSelfTest(const char SelfTest) {
    if ((SelfTest & ACC_CTRL_REG5_SELF_TEST_PROHIBITED) 
            != ACC_CTRL_REG5_SELF_TEST_PROHIBITED) { //Avoid not allowed state
        ACC.CTRL_REG5 = (ACC.CTRL_REG5 & ~ACC_SELF_TEST_MASK)
                | (SelfTest & ACC_SELF_TEST_MASK);
    }
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
    //TODO: Need to finish this function
}

void vAccSetFIFOMode(const char FIFOMode) {
    ACC.FIFO_CTRL = (ACC.FIFO_CTRL & ~ACC_FIFO_MODE_MASK)
            | (FIFOMode & ACC_FIFO_MODE_MASK);
    vSpiWriteByte(ACC_ADDR_FIFO_CTRL, ACC.FIFO_CTRL);
}

void vAccSetSPIMode(const char SPIMode) {
    ACC.CTRL_REG5 = (ACC.CTRL_REG5 & ~(ACC_SPI_MODE_3WIRE))
            | (SPIMode & ACC_SPI_MODE_3WIRE);
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
}

void vAccBlockDataUpdate(int block) {
    block ? ACC.CTRL_REG4 |= 1 << 3 : ACC.CTRL_REG4 &= ~(1 << 3);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableAxis(const char Axis, int enable) {
    enable ? ACC.CTRL_REG4 |= (Axis & ACC_AXIS_MASK)
            : ACC.CTRL_REG4 &= ~(Axis & ACC_AXIS_MASK);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableFIFO(int enable) {
    enable ? ACC.CTRL_REG6 |= 1 << 6 : ACC.CTRL_REG6 &= ~(1 << 6);
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccEnableAddrInc(int enable) {
    enable ? ACC.CTRL_REG6 |= 1 << 4 : ACC.CTRL_REG6 &= ~(1 << 4);
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

int iAccIsDataOverrun() {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & (1 << 7));
}

int iAccIsDataOverrun(const char Axis) {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & ((Axis & ACC_AXIS_MASK) << 4));
}

int iAccIsDataReady() {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & (1 << 3));
}

int iAccIsDataReady(const char Axis) {
    return (int) (cSpiReadByte(ACC_ADDR_STATUS) & (Axis & ACC_AXIS_MASK));
}

int iAccIsFIFOFilled() {
    return (int) (cSpiReadByte(ACC_ADDR_FIFO_SRC) & (1 << 6));
}

int iAccIsFIFOEmpty() {
    return (int) (cSpiReadByte(ACC_ADDR_FIFO_SRC) & (1 << 5));
}

char cAccGetINFO(char infoReg) {
    return cSpiReadByte(infoReg);
}

