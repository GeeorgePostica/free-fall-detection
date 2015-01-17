/* 
 * File:   acc_controls.h
 * Author: george
 *
 * Created on January 13, 2015, 11:46 PM
 */

#ifndef ACC_CONTROLS_H
#define	ACC_CONTROLS_H

#define ACC_G           9.8
#define ACC_SCALE_2G    2
#define ACC_SCALE_4G    4
#define ACC_SCALE_6G    6
#define ACC_SCALE_8G    8
#define ACC_SCALE_16G   16

typedef struct{
    float scale;
    float freq;
    int fifo;
} ACC_StatusType;

void vAccInit();
float fAccGetX();       /* Get the acceleration along X-axis. [m/s^2] */
float fAccGetY();       /* Get the acceleration along Y-axis. [m/s^2] */
float fAccGetZ();       /* Get the acceleration along Z-axis. [m/s^2] */
float* pfAccGetXYZ();   /* Get the acceleration along all axis. [m/s^2] */
void vAccSoftReset();
void vAccReboot();
void vAccSetScale(const char scale);
void vAccSetRate(const char rate);
//void vAccSelfTest(const char SelfTest);
void vAccEnableAxis(const char Axis, int enable);
void vAccEnableFIFO(int enable);
int iAccIsDataOverrun();
int iAccIsDataOverrun(const char Axis);
int iAccIsDataReady();
int iAccIsDataReady(const char Axis);
int iAccIsFIFOFilled();
int iAccIsFIFOEmpty();

#endif	/* ACC_CONTROLS_H */

