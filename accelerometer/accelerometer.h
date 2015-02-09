/* 
 * File:   accelerometer.h
 * Author: george
 *
 * Created on February 9, 2015, 12:48 AM
 */

#ifndef ACCELEROMETER_H
#define	ACCELEROMETER_H

#include <miosix.h>
#include "protocols/spi.h"
#include "acc_registers.h"
#include "config.h"

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

class Axis{
public:
    enum Axis_{
        X = ACC_AXIS_X,
        Y = ACC_AXIS_Y,
        Z = ACC_AXIS_Z
    };
};

class ODR{
public:
    enum ODR_{
        _3_125_Hz = ACC_CTRL_REG4_ODR_3_125Hz,
        _6_25_Hz = ACC_CTRL_REG4_ODR_6_250Hz,
        _12_5_Hz = ACC_CTRL_REG4_ODR_12_500Hz,
        _25_Hz = ACC_CTRL_REG4_ODR_25Hz,
        _50_Hz = ACC_CTRL_REG4_ODR_50Hz,
        _100_Hz = ACC_CTRL_REG4_ODR_100Hz,
        _400_Hz = ACC_CTRL_REG4_ODR_400Hz,
        _800_Hz = ACC_CTRL_REG4_ODR_800Hz,
        _1600_Hz = ACC_CTRL_REG4_ODR_1600Hz
    };
};

class FScale{
public:
    enum Scale{
        _2G = ACC_CTRL_REG5_FSCALE_2G,
        _4G = ACC_CTRL_REG5_FSCALE_4G,
        _6G = ACC_CTRL_REG5_FSCALE_6G,
        _8G = ACC_CTRL_REG5_FSCALE_8G,
        _16G = ACC_CTRL_REG5_FSCALE_16G
    };
};

class Accelerometer {
public:
    /* Accelerometer static functions */
    static void init();
    static float getX(); /* Get the acceleration along X-axis. [m/s^2] */
    static float getY(); /* Get the acceleration along Y-axis. [m/s^2] */
    static float getZ(); /* Get the acceleration along Z-axis. [m/s^2] */
    static float* getXYZ(float xyz[], int samplesToAvg); /* Get the acceleration along all axis. [m/s^2] */
    static void reset();
    static void reboot();
    static void setScale(FScale::Scale scale);
    static void setRate(ODR::ODR_ odr);
    static void enableAxis(Axis::Axis_ axis, bool enable);
    static void setOffset(Axis::Axis_ axis, char offset);
    static char getINFO(char infoReg);
    static void disconnect();
    
private:
    static bool initialized;
    
    /*---------------- REGISTER CONTAINERS -------------------*/
    static char CTRL_REG4;   /* |bit|def | name | info
                              * -----------------------------------------------
                              * |7:4|0000| ODR  | Operating data and power rate
                              * | 3 | 0  | BDU  | Block data update.
                              * | 2 | 1  | ZEN  | Z axis enable.
                              * | 1 | 1  | YEN  | Y axis enable.
                              * | 0 | 1  | XEN  | X axis enable.    */

    static char CTRL_REG3;   /* |bit|def|   name    | info
                              * -------------------------------------------------
                              * | 7 | 0 |   DR_EN   | DRDY signal enable to INT1.
                              * | 6 | 0 |   IEA     | Interrupt signal polarity.
                              * | 5 | 0 |   IEL     | Interrupt signal latching
                              * | 4 | 0 |   INT2_EN | Interrupt 2 enable/disable
                              * | 3 | 0 |   INT1_EN | Interrupt 1 enable/disable.
                              * | 2 | 0 |   VLIFT   | Vector filter enable/disable
                              * | 0 | - |   STRT    | Soft reset bit    */

    static char CTRL_REG5;   /* |bit|def|   name    | info
                              * --------------------------------------
                              * |7:6| 00|   BW2:BW1 | Anti-aliasing filter bandwidth.
                              * |5:3|000|  FSCALE2:0| Full-scale selection.
                              * |2:1| 00|   ST2:1   | Self-test enable.
                              * | 0 | 0 |   SIM     | SPI serial interface mode selection.*/

    static char CTRL_REG6;   /* |bit|def|   name    | info
                              * --------------------------------------
                              * | 7 | - |   BOOT    | Force reboot, cleared when finish.
                              * | 6 | 0 |   FIFO_EN | FIFO enable.
                              * | 5 | 0 |   WTM_EN  | Enable FIFO Watermark level use.
                              * | 4 | - |   ADD_INC | Automatically incremented 
                              *                       during a multiple byte access 
                              *                       with a serial interface (I2C or SPI). 
                              * | 3 | 0 |   P1_EMPTY| Enable FIFO Empty indication on int1.
                              * | 2 | 0 |   P1_WTM  | FIFO Watermark interrupt on int1. 
                              * | 1 | 0 | P1_OVERRUN| FIFO overrun interrupt on int1.
                              * | 0 | 0 |   BOOT_2  | BOOT interrupt on int2. */


    static char FIFO_CTRL;   /* |bit|def | name | info
                              * -----------------------------------------
                              * |7:5|--- |FMODE | FIFO Mode Selection.
                              * |4:0|----| WTMP | FIFO Watermark pointer. 
                              * -----------------------------------------
                              * FIFO Modes:
                              * 000: Bypass Mode. FIFO turned off
                              * 001: FIFO Mode. Stop collecting data when FIFO is full.
                              * 010: Stream Mode. If the FIFO is full the new sample 
                              *      overwrites the older one   
                              * 011: Stream mode until trigger is de-asserted, 
                              *      then FIFO mode   
                              * 100: Bypass mode until trigger is de-asserted, 
                              *      then Stream mode   
                              * 111: Bypass mode until trigger is de-asserted, 
                              *      then FIFO mode    */

    static float fscale; /* Full scale multiplier */
    static char offset; /* Offset multiplier */

    static bool isDataReady();
    static bool isDataReady(Axis::Axis_ axis);
};

#endif	/* ACCELEROMETER_H */

