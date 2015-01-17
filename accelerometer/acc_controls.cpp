#include "acc_registers.h"
#include "acc_controls.h"
#include "protocols/spi_acc.h"

ACC_StatusType xAccStatus;

/************ FUNCTION PROTOTYPES ***********/
short sAccGetAxis(char axisRegAddress);
void vAccSetFIFOMode(const char FIFOMode);
void vAccBlockDataUpdate(int block);
void vAccEnableAddrInc(int enable);
/********************************************/


void vAccInit(){
    vSpiInit();
    //vAccRegSetSPIMode(ACC_SPI_MODE_4WIRE);
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
    vAccSoftReset();
}

void vAccReboot(){
    vAccRegForceReboot();
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccSoftReset(){
    //vAccRegSoftReset();       //DUE TO A STRANGE LINKER ERROR
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
    vAccEnableAddrInc(1); 
    vSpiReadShort(ACC_ADDR_OUT_X_L, res);
    vAccEnableAddrInc(0);
    vAccBlockDataUpdate(0);
    return res;
}

float* pfAccGetXYZ(float xyz[]){
    short values[3];
    vAccBlockDataUpdate(1);
    vAccEnableAddrInc(1); 
    vSpiReadArrayShort(ACC_ADDR_OUT_X_L, values, 3);
    vAccEnableAddrInc(0);
    vAccBlockDataUpdate(0);
    xyz[0] = values[2] * xAccStatus.scale;    /*Order reversed due to SPI protocol*/
    xyz[1] = values[1] * xAccStatus.scale;
    xyz[2] = values[0] * xAccStatus.scale;
    return xyz;
}

void vAccSetScale(int scale){
    xAccStatus.scale = 1/(scale*ACC_G);
    switch(scale){
        case ACC_SCALE_4G:  vAccRegSetFScale(ACC_FSCALE_4G);
                            break;
        case ACC_SCALE_6G:  vAccRegSetFScale(ACC_FSCALE_6G);
                            break;
        case ACC_SCALE_8G:  vAccRegSetFScale(ACC_FSCALE_8G);
                            break;
        case ACC_SCALE_16G: vAccRegSetFScale(ACC_FSCALE_16G);
                            break;
        default:            vAccRegSetFScale(ACC_FSCALE_2G);
                            xAccStatus.scale = 1/(2*ACC_G);
                            break;
    }
    vSpiWriteByte(ACC_ADDR_CTRL_REG5, ACC.CTRL_REG5);
}

void vAccSetRate(char rate){
    vAccRegSetODR(rate);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccSetFIFOMode(const char FIFOMode){
    vAccRegSetFIFOMode(FIFOMode);
    vSpiWriteByte(ACC_ADDR_FIFO_CTRL, ACC.FIFO_CTRL);
}

void vAccBlockDataUpdate(int block){
    block? vAccRegBlockDataUpdate() : vAccRegEnableDataUpdate();
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableAxis(const char Axis, int enable){
    enable ? vAccRegEnableAxis(Axis) : vAccRegDisableAxis(Axis);
    vSpiWriteByte(ACC_ADDR_CTRL_REG4, ACC.CTRL_REG4);
}

void vAccEnableFIFO(int enable){
    enable ? vAccRegEnableFIFO() : vAccRegDisableFIFO();
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

void vAccEnableAddrInc(int enable){
    enable ? vAccRegAddrIncON() : vAccRegAddrIncOFF();
    vSpiWriteByte(ACC_ADDR_CTRL_REG6, ACC.CTRL_REG6);
}

int iAccIsDataOverrun(){
    vSpiReadByte(ACC_ADDR_STATUS, ACC.STATUS);
    return (int)iAccRegIsDataOverrun();
}

int iAccIsDataOverrun(const char Axis){
    vSpiReadByte(ACC_ADDR_STATUS, ACC.STATUS);
    return (int)iAccRegIsDataOverrunAxis(Axis);
}

int iAccIsDataReady(){
    vSpiReadByte(ACC_ADDR_STATUS, ACC.STATUS);
    return (int)iAccRegIsDataReady();
}

int iAccIsDataReady(const char Axis){
    vSpiReadByte(ACC_ADDR_STATUS, ACC.STATUS);
    return (int)iAccRegIsDataReadyAxis(Axis);
}

int iAccIsFIFOFilled(){
    vSpiReadByte(ACC_ADDR_FIFO_SRC, ACC.FIFO_SRC);
    return (int)iAccIsFIFOFilled();
}

int iAccIsFIFOEmpty(){
    vSpiReadByte(ACC_ADDR_FIFO_SRC, ACC.FIFO_SRC);
    return (int)iAccIsFIFOEmpty();
}

