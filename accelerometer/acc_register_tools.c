#include "acc_registers.h"

//#define LOCAL_ACC_FUNCTIONS
#ifndef LOCAL_ACC_FUNCTIONS        /* Define it to have different set of functions*/

/************ Accelerometer registers control functions **********************/

void vAccRegSetODR(const char ODR){
    ACC->CTRL_REG4 = (ACC->CTRL_REG4 & ~ACC_ODR_MASK) 
                   | (ODR & ACC_ODR_MASK);
}

void vAccRegSetFScale(const char FScale){
    ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~ACC_FSCALE_MASK) 
                   | (FScale & ACC_FSCALE_MASK);
}

void vAccRegSelfTest(const char SelfTest){
    if((SelfTest & ACC_SELF_TEST_PROHIBITED) != ACC_SELF_TEST_PROHIBITED){ //Avoid not allowed state
        ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~ACC_SELF_TEST_MASK) 
                       | (SelfTest & ACC_SELF_TEST_MASK);
    }
}

void vAccRegSetFIFOMode(const char FIFOMode){
    ACC->FIFO_CTRL = (ACC->FIFO_CTRL & ~ACC_FIFO_MODE_MASK) 
                   | (FIFOMode & ACC_FIFO_MODE_MASK);
}

void vAccRegBlockDataUpdate(){
    ACC->CTRL_REG4 |= 1 << 3;
}

void vAccRegEnableAxis(const char Axis){
    ACC->CTRL_REG4 |= (Axis & ACC_AXIS_MASK);
}

void vAccRegDisableAxis(const char Axis){
    ACC->CTRL_REG4 &= ~(Axis & ACC_AXIS_MASK);
}

void vAccRegSoftReset(){
    ACC->CTRL_REG3 |= 1;
}

void vAccRegSetSPIMode(const char SPIMode){
    ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~(ACC_SPI_MODE_3WIRE))
                   | (SPIMode & ACC_SPI_MODE_3WIRE);
}

void vAccRegEnableFIFO(){
    ACC->CTRL_REG6 |= 1 << 6;
}

void vAccRegDisableFIFO(){
    ACC->CTRL_REG6 &= ~(1 << 6);
}

void vAccRegForceReboot(){
    ACC->CTRL_REG6 |= 1 << 7;
}

void vAccRegAddrIncON(){
    ACC->CTRL_REG6 |= 1 << 4;
}

void vAccRegAddrIncOFF(){
    ACC->CTRL_REG6 &= ~(1 << 4);
}

int iAccRegIsDataOverrun(){
    return ACC->STATUS & (1 << 7);
}

int iAccRegIsDataOverrun(const char Axis){
    return ACC->STATUS & ((Axis & ACC_AXIS_MASK) << 4);
}

int iAccRegIsDataReady(){
    return ACC->STATUS & (1 << 3);
}

int iAccRegIsDataReady(const char Axis){
    return ACC->STATUS & (Axis & ACC_AXIS_MASK);
}

int iAccRegIsFIFOFilled(){
    return ACC->FIFO_SRC & (1 << 6);
}

int iAccRegIsFIFOEmpty(){
    return ACC->FIFO_SRC & (1 << 5);
}

#else
void vAccSetODR(ACC_TypeDef &ACC, const char ODR){
    ACC->CTRL_REG4 = (ACC->CTRL_REG4 & ~ACC_ODR_MASK) 
                   | (ODR & ACC_ODR_MASK);
}

void vAccSetFScale(ACC_TypeDef &ACC, const char FScale){
    ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~ACC_FSCALE_MASK) 
                   | (FScale & ACC_FSCALE_MASK);
}

void vAccSelfTest(ACC_TypeDef &ACC, const char SelfTest){
    if((SelfTest & ACC_SELF_TEST_PROHIBITED) != ACC_SELF_TEST_PROHIBITED){ //Avoid not allowed state
        ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~ACC_SELF_TEST_MASK) 
                       | (SelfTest & ACC_SELF_TEST_MASK);
    }
}

void vAccSetFIFOMode(ACC_TypeDef &ACC, const char FIFOMode){
    ACC->FIFO_CTRL = (ACC->FIFO_CTRL & ~ACC_FIFO_MODE_MASK) 
                   | (FIFOMode & ACC_FIFO_MODE_MASK);
}

void vAccBlockDataUpdate(ACC_TypeDef &ACC){
    ACC->CTRL_REG4 |= 1 << 3;
}

void vAccEnableAxis(ACC_TypeDef &ACC, const char Axis){
    ACC->CTRL_REG4 |= (Axis & ACC_AXIS_MASK);
}

void vAccDisableAxis(ACC_TypeDef &ACC, const char Axis){
    ACC->CTRL_REG4 &= ~(Axis & ACC_AXIS_MASK);
}

void vAccSoftReset(ACC_TypeDef &ACC){
    ACC->CTRL_REG3 |= 1;
}

void vAccSetSPIMode(ACC_TypeDef &ACC, const char SPIMode){
    ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~(ACC_SPI_MODE_3WIRE))
                   | (SPIMode & ACC_SPI_MODE_3WIRE);
}

void vAccEnableFIFO(ACC_TypeDef &ACC){
    ACC->CTRL_REG6 |= 1 << 6;
}

void vAccDisableFIFO(ACC_TypeDef &ACC){
    ACC->CTRL_REG6 &= ~(1 << 6);
}

void vAccForceReboot(ACC_TypeDef &ACC){
    ACC->CTRL_REG6 |= 1 << 7;
}

void vAccAddrIncON(ACC_TypeDef &ACC){
    ACC->CTRL_REG6 |= 1 << 4;
}

void vAccAddrIncOFF(ACC_TypeDef &ACC){
    ACC->CTRL_REG6 &= ~(1 << 4);
}

int iAccIsDataOverrun(ACC_TypeDef ACC){
    return ACC->STATUS & (1 << 7);
}

int iAccIsDataOverrun(ACC_TypeDef &ACC, const char Axis){
    return ACC->STATUS & ((Axis & ACC_AXIS_MASK) << 4);
}

int iAccIsDataReady(ACC_TypeDef ACC){
    return ACC->STATUS & (1 << 3);
}

int iAccIsDataReady(ACC_TypeDef &ACC, const char Axis){
    return ACC->STATUS & (Axis & ACC_AXIS_MASK);
}

int iAccIsFIFOFilled(ACC_TypeDef ACC){
    return ACC->FIFO_SRC & (1 << 6);
}

int iAccIsFIFOEmpty(ACC_TypeDef ACC){
    return ACC->FIFO_SRC & (1 << 5);
}

#endif