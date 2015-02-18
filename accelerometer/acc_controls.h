/* 
 * File:   acc_controls.h
 * Author: george
 *
 * Created on January 13, 2015, 11:46 PM
 */

#ifndef ACC_CONTROLS_H
#define	ACC_CONTROLS_H

#include "config.h"
#include "acc_lis3dsh.h"

/* Full Scale multipliers */
#define ACC_SCALE_2G    ACC_SCALE_TO_FLOAT_2G * ACC_G
#define ACC_SCALE_4G    ACC_SCALE_TO_FLOAT_4G * ACC_G
#define ACC_SCALE_6G    ACC_SCALE_TO_FLOAT_6G * ACC_G
#define ACC_SCALE_8G    ACC_SCALE_TO_FLOAT_8G * ACC_G
#define ACC_SCALE_16G   ACC_SCALE_TO_FLOAT_16G * ACC_G

#ifndef ACC_PRINT_INIT_CONFIG
#define ACC_PRINT_INIT_CONFIG   " "
#endif

/* Accelerometer function prototypes */
/**
 * Initializes the accelerometer which initializes the SPI first and then
 * sends all the initial configuration to the accelerometer via SPI
 * @return 1 if initialization succeeded or 0 otherwise
 */
int iAccInit();

/**
 * Get the acceleration on X axis in m/s^2
 * @return acceleration [m/s^2] on X 
 */
float fAccGetX();

/**
 * Get the acceleration on Y axis in m/s^2
 * @return acceleration [m/s^2] on Y 
 */
float fAccGetY();

/**
 * Get the acceleration on Z axis in m/s^2
 * @return acceleration [m/s^2] on Z 
 */
float fAccGetZ();

/**
 * Get the acceleration [m/s^2] on all three axis averaged over
 * a number of samples
 * @param xyz the array where to store the result <i>Note</i>: should have 
 * a size >= 3
 * @param samples the number of samples for averaging
 */
void vAccGetXYZ(float xyz[], int samples);

/**
 * Execute a soft reset of the accelerometer
 */
void vAccSoftReset();

/**
 * Reboot the accelerometer <br>
 * <i>Note</i>: the accelerometer will be configured with the initial
 * configuration with which it was started
 */
void vAccReboot();

/**
 * Set the operating scale in G
 * @param scale - the full scale bit mask according to data sheet <br>
 * For a simpler use, consider storing the masks in a header file
 */
void vAccSetScale(const char scale);  // (E.g. see acc_lis3dsh.h for masks)

/**
 * Set the operating data rate (ODR)
 * @param rate - the ODR bit mask according to data sheet <br>
 * For a simpler use, consider storing the masks in a header file
 */
void vAccSetRate(const char rate); // (E.g. see acc_lis3dsh.h for masks)

/**
 * Enable the specified axis
 * @param Axis - the bit mask to select the axis
 * @param enable - 1: enable, 0: disable
 */
void vAccEnableAxis(const char Axis, int enable);

/**
 * Check if data was overrun on all three axis
 * @return 0 if data was not overrun, 1 otherwise
 */
int iAccIsDataOverrun();

/**
 * Check if data was overrun on specific axis
 * @param Axis - the axis bit mask
 * @return 1 if data was overrun, 0 otherwise
 */
int iAccIsDataOverrun(const char Axis);

/**
 * Check if a new set of data is ready on every axis
 * @return 1 if a new set of data is ready, 0 otherwise
 */
int iAccIsDataReady();

/**
 * Check if a new set of data is ready on the specified axis
 * @param Axis - the axis bit mask
 * @return 1 if new data is available, 0 otherwise
 */
int iAccIsDataReady(const char Axis);

/**
 * Set the offset over X axis. See data sheet how the offset is calculated
 * @param offset - the amount of offset for X axis
 */
void vAccSetOffsetX(char offset);

/**
 * Set the offset over Y axis. See data sheet how the offset is calculated
 * @param offset - the amount of offset for Y axis
 */
void vAccSetOffsetY(char offset);

/**
 * Set the offset over Z axis. See data sheet how the offset is calculated
 * @param offset - the amount of offset for Z axis
 */
void vAccSetOffsetZ(char offset);

/**
 * Read a value from the specified register address
 * @param infoReg - the address of the register to read from
 * @return the value read from the register
 */
char cAccGetINFO(char infoReg);

/**
 * Stop the accelerometer along with the SPI it uses
 */
void vAccStop();

#endif	/* ACC_CONTROLS_H */

