#include "acc_controls.h"
#include "acc_registers.h"
#include "protocols/spi_acc.h"

ACC_StatusType xAccStatus;
ACC_TypeDef ACC;            /* The data structure which represents the accelerometer */

/************ FUNCTION PROTOTYPES ***********/
short sAccGetAxis(char axisRegAddress);
void vAccSetFIFOMode(const char FIFOMode);
void vAccBlockDataUpdate(int block);
void vAccEnableAddrInc(int enable);
void vAccSetSPIMode(const char SPIMode);
/********************************************/

void vAccInit(){
    vSpiInit();
    
    //vAccReboot();
    //vSpiWriteByte(ACC_ADDR_CTRL_REG3, 0x67);
    //vAccSetRate(ACC_ODR_800Hz);
    //vAccSetSPIMode(ACC_SPI_MODE_4WIRE);
    //vAccReboot();
    cSpiReadByte(ACC_ADDR_STATUS);
    //vSpiWriteByte(ACC_ADDR_CTRL_REG3, 0xC9); // resetting the whole internal circuit (0xC9 for interrupts)
    vAccSoftReset();
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, 0x67); // 100Hz data update rate, block data update disable, x/y/z enabled 
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, 0x21); // Anti aliasing filter bandwidth 800Hz, 16G (very sensitive), no self-test, 4-wire interface
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, 0x10); // Enable auto increment
    //vAccSoftReset();
}

void vAccReboot(){
    ACC.CTRL_REG6 |= 1 << 7;
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccSoftReset(){
    ACC.CTRL_REG3 |= 1;
    vSpiWriteByte(ACC_ADDR_CTRL_REG3, ACC.CTRL_REG3);
}

float fAccGetX(){
    return sAccGetAxis(ACC_ADDR_OUT_X_L) * xAccStatus.scale;
}

float fAccGetY(){
    return sAccGetAxis(ACC_ADDR_OUT_Y_L) * xAccStatus.scale;
}

float fAccGetZ(){
    return sAccGetAxis(ACC_ADDR_OUT_Z_L) * xAccStatus.scale;
}

short sAccGetAxis(char axisRegAddress){
    short res;
    vAccBlockDataUpdate(1);
    res = sSpiReadShort(axisRegAddress);
    vAccBlockDataUpdate(0);
    return res;
}

float* pfAccGetXYZ(float xyz[]){
    short values[3];
    vAccBlockDataUpdate(1);
    vSpiReadArrayShort(ACC_ADDR_OUT_X_L, values, 3);
    vAccBlockDataUpdate(0);
    xyz[0] = values[2] * xAccStatus.scale;    /*Order reversed due to SPI protocol*/
    xyz[1] = values[1] * xAccStatus.scale;
    xyz[2] = values[0] * xAccStatus.scale;
    return xyz;
}

void vAccSetScale(const char scale){
    ACC.CTRL_REG5 = (ACC.CTRL_REG5 & ~ACC_FSCALE_MASK) 
                   | (scale & ACC_FSCALE_MASK);
    switch(scale){
        case ACC_CTRL_REG5_FSCALE_4G:     xAccStatus.scale = 1/(ACC_SCALE_4G*ACC_G);
                                          break;
        case ACC_CTRL_REG5_FSCALE_6G:     xAccStatus.scale = 1/(ACC_SCALE_6G*ACC_G);
                                          break;
        case ACC_CTRL_REG5_FSCALE_8G:     xAccStatus.scale = 1/(ACC_SCALE_8G*ACC_G);
                                          break;
        case ACC_CTRL_REG5_FSCALE_16G:    xAccStatus.scale = 1/(ACC_SCALE_16G*ACC_G);
                                          break;
        default:                xAccStatus.scale = 1/(ACC_SCALE_2G*ACC_G);
                                break;
    }
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
}

void vAccSetRate(const char rate){
    ACC.CTRL_REG4 = (ACC.CTRL_REG4 & ~ACC_ODR_MASK) 
                   | (rate & ACC_ODR_MASK);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccSelfTest(const char SelfTest){
    if((SelfTest & ACC_CTRL_REG5_SELF_TEST_PROHIBITED) != ACC_CTRL_REG5_SELF_TEST_PROHIBITED){ //Avoid not allowed state
        ACC.CTRL_REG5 = (ACC.CTRL_REG5 & ~ACC_SELF_TEST_MASK) 
                       | (SelfTest & ACC_SELF_TEST_MASK);
    }
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
    //TODO: Need to finish this function
}

void vAccSetFIFOMode(const char FIFOMode){
    ACC.FIFO_CTRL = (ACC.FIFO_CTRL & ~ACC_FIFO_MODE_MASK) 
                   | (FIFOMode & ACC_FIFO_MODE_MASK);
    vSpiWriteByte(ACC_ADDR_FIFO_CTRL, ACC.FIFO_CTRL);
}

void vAccSetSPIMode(const char SPIMode){
    ACC.CTRL_REG5 = (ACC.CTRL_REG5 & ~(ACC_SPI_MODE_3WIRE))
                   | (SPIMode & ACC_SPI_MODE_3WIRE);
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
}

void vAccBlockDataUpdate(int block){
    block? ACC.CTRL_REG4 |= 1 << 3 : ACC.CTRL_REG4 &= ~(1 << 3);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableAxis(const char Axis, int enable){
    enable ? ACC.CTRL_REG4 |= (Axis & ACC_AXIS_MASK)
            : ACC.CTRL_REG4 &= ~(Axis & ACC_AXIS_MASK);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableFIFO(int enable){
    enable ? ACC.CTRL_REG6 |= 1 << 6 : ACC.CTRL_REG6 &= ~(1 << 6);
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccEnableAddrInc(int enable){
    enable ? ACC.CTRL_REG6 |= 1 << 4 : ACC.CTRL_REG6 &= ~(1 << 4);
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

int iAccIsDataOverrun(){
    ACC.STATUS = cSpiReadByte(ACC_ADDR_STATUS);
    return (int)(ACC.STATUS & (1 << 7));
}

int iAccIsDataOverrun(const char Axis){
    ACC.STATUS = cSpiReadByte(ACC_ADDR_STATUS);
    return (int)(ACC.STATUS & ((Axis & ACC_AXIS_MASK) << 4));
}

int iAccIsDataReady(){
    ACC.STATUS = cSpiReadByte(ACC_ADDR_STATUS);
    return (int)(ACC.STATUS & (1 << 3));
}

int iAccIsDataReady(const char Axis){
    ACC.STATUS = cSpiReadByte(ACC_ADDR_STATUS);
    return (int)(ACC.STATUS & (Axis & ACC_AXIS_MASK));
}

int iAccIsFIFOFilled(){
    ACC.FIFO_SRC = cSpiReadByte(ACC_ADDR_FIFO_SRC);
    return (int)(ACC.FIFO_SRC & (1 << 6));
}

int iAccIsFIFOEmpty(){
    ACC.FIFO_SRC = cSpiReadByte(ACC_ADDR_FIFO_SRC);
    return (int)(ACC.FIFO_SRC & (1 << 5));
}

char cAccGetINFO(char infoReg){
    return cSpiReadByte(infoReg);
}

