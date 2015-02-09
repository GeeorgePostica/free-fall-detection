/* 
 * File:   acc_controls.h
 * Author: george
 *
 * Created on January 13, 2015, 11:46 PM
 */

#ifndef ACC_CONTROLS_H
#define	ACC_CONTROLS_H

#include "config.h"
#include "acc_registers.h"

/* Full Scale multipliers */
#define ACC_G           9.8
#define ACC_S_RATIO     1.0/32768.0
#define ACC_SCALE_2G    2.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_4G    4.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_6G    6.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_8G    8.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_16G   24.0 * ACC_S_RATIO * ACC_G  //It seems like it is a bug
/* Default offsets based on scale */
#define ACC_OFFSET_2G   1
#define ACC_OFFSET_4G   2
#define ACC_OFFSET_6G   3
#define ACC_OFFSET_8G   4
#define ACC_OFFSET_16G  16

/* Offset values: vary on each accelerometer*/
#define ACC_OFFSET_X    0x0C
#define ACC_OFFSET_Y    0x04
#define ACC_OFFSET_Z    0x0C


/* Accelerometer function prototypes */
void vAccInit();
float fAccGetX();       /* Get the acceleration along X-axis. [m/s^2] */
float fAccGetY();       /* Get the acceleration along Y-axis. [m/s^2] */
float fAccGetZ();       /* Get the acceleration along Z-axis. [m/s^2] */
void vAccGetXYZ(float xyz[]);   /* Get the acceleration along all axis. [m/s^2] */
void vAccGetAverageXYZ(float xyz[], int samples); /* Get the averaged values */
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

void vAccSetOffsetX(char offset);
void vAccSetOffsetY(char offset);
void vAccSetOffsetZ(char offset);

char cAccGetINFO(char infoReg);


#endif	/* ACC_CONTROLS_H */

