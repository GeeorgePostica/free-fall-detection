/* 
 * File:   acc_controls.h
 * Author: george
 *
 * Created on January 13, 2015, 11:46 PM
 */

#ifndef ACC_CONTROLS_H
#define	ACC_CONTROLS_H

#include "acc_registers.h"

#define ACC_G           9.8
#define ACC_S_RATIO     1.0/32768.0
#define ACC_SCALE_2G    2.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_4G    4.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_6G    6.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_8G    8.0 * ACC_S_RATIO * ACC_G
#define ACC_SCALE_16G   16.0 * ACC_S_RATIO * ACC_G

typedef struct{
    char CTRL_REG4;          /* |bit|def | name | info
                              * -----------------------------------------------
                              * |7:4|0000| ODR  | Operating data and power rate
                              * | 3 | 0  | BDU  | Block data update.
                              * | 2 | 1  | ZEN  | Z axis enable.
                              * | 1 | 1  | YEN  | Y axis enable.
                              * | 0 | 1  | XEN  | X axis enable.    */
    
    char CTRL_REG3;          /* |bit|def|   name    | info
                              * -------------------------------------------------
                              * | 7 | 0 |   DR_EN   | DRDY signal enable to INT1.
                              * | 6 | 0 |   IEA     | Interrupt signal polarity.
                              * | 5 | 0 |   IEL     | Interrupt signal latching
                              * | 4 | 0 |   INT2_EN | Interrupt 2 enable/disable
                              * | 3 | 0 |   INT1_EN | Interrupt 1 enable/disable.
                              * | 2 | 0 |   VLIFT   | Vector filter enable/disable
                              * | 0 | - |   STRT    | Soft reset bit    */
    
    char CTRL_REG5;          /* |bit|def|   name    | info
                              * --------------------------------------
                              * |7:6| 00|   BW2:BW1 | Anti-aliasing filter bandwidth.
                              * |5:3|000|  FSCALE2:0| Full-scale selection.
                              * |2:1| 00|   ST2:1   | Self-test enable.
                              * | 0 | 0 |   SIM     | SPI serial interface mode selection.*/
    
    char CTRL_REG6;          /* |bit|def|   name    | info
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
    
        
    char FIFO_CTRL;          /* |bit|def | name | info
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
    
    float scale;             /* Full scale multiplier */
    
} ACC_TypeDef;

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

char cAccGetINFO(char infoReg);


#endif	/* ACC_CONTROLS_H */

