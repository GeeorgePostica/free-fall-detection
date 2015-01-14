#include "acc_registers.h"

//#define LOCAL_ACC_FUNCTIONS
#ifndef LOCAL_ACC_FUNCTIONS        /* Define it to have different set of functions*/

ACC_TypeDef ACC;

/************ Accelerometer registers control functions **********************/

void vAccSetODR(const char ODR){
    ACC->CTRL_REG4 = (ACC->CTRL_REG4 & ~ACC_ODR_MASK) 
                   | (ODR & ACC_ODR_MASK);
}

void vAccSetFScale(const char FScale){
    ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~ACC_FSCALE_MASK) 
                   | (FScale & ACC_FSCALE_MASK);
}

void vAccSelfTest(const char SelfTest){
    if((SelfTest & ACC_SELF_TEST_PROHIBITED) != ACC_SELF_TEST_PROHIBITED){ //Avoid not allowed state
        ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~ACC_SELF_TEST_MASK) 
                       | (SelfTest & ACC_SELF_TEST_MASK);
    }
}

void vAccSetFIFOMode(const char FIFOMode){
    ACC->FIFO_CTRL = (ACC->FIFO_CTRL & ~ACC_FIFO_MODE_MASK) 
                   | (FIFOMode & ACC_FIFO_MODE_MASK);
}

void vAccBlockDataUpdate(){
    ACC->CTRL_REG4 |= 1 << 3;
}

void vAccEnableAxis(const char Axis){
    ACC->CTRL_REG4 |= (Axis & ACC_AXIS_MASK);
}

void vAccDisableAxis(const char Axis){
    ACC->CTRL_REG4 &= ~(Axis & ACC_AXIS_MASK);
}

void vAccSoftReset(){
    ACC->CTRL_REG3 |= 1;
}

void vAccSetSPIMode(const char SPIMode){
    ACC->CTRL_REG5 = (ACC->CTRL_REG5 & ~(ACC_SPI_MODE_3WIRE))
                   | (SPIMode & ACC_SPI_MODE_3WIRE);
}

void vAccEnableFIFO(){
    ACC->CTRL_REG6 |= 1 << 6;
}

void vAccDisableFIFO(){
    ACC->CTRL_REG6 &= ~(1 << 6);
}

void vAccForceReboot(){
    ACC->CTRL_REG6 |= 1 << 7;
}

void vAccAddrIncON(){
    ACC->CTRL_REG6 |= 1 << 4;
}

void vAccAddrIncOFF(){
    ACC->CTRL_REG6 &= ~(1 << 4);
}

int iAccIsDataOverrun(){
    return ACC->STATUS & (1 << 7);
}

int iAccIsDataOverrun(const char Axis){
    return ACC->STATUS & ((Axis & ACC_AXIS_MASK) << 4);
}

int iAccIsDataReady(){
    return ACC->STATUS & (1 << 3);
}

int iAccIsDataReady(const char Axis){
    return ACC->STATUS & (Axis & ACC_AXIS_MASK);
}

int iAccIsFIFOFilled(){
    return ACC->FIFO_SRC & (1 << 6);
}

int iAccIsFIFOEmpty(){
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