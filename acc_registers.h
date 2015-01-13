/* 
 * File:   acc_registers.h
 * Author: george
 *
 * Created on January 13, 2015, 1:21 PM
 */

#ifndef ACC_REGISTERS_H
#define	ACC_REGISTERS_H


#include <stdint.h>

#define ACC ((ACC_TypeDef *) 0x20)

#define HZ50 0x50

typedef struct{
    volatile char CTRL_REG4; /* |bit|def | name | info
                              * -----------------------------------------------
                              * |7:4|0000| ODR  | Operating data and power rate
                              * | 3 | 0  | BDU  | Block data update.
                              * | 2 | 1  | ZEN  | Z axis enable.
                              * | 1 | 1  | YEN  | Y axis enable.
                              * | 0 | 1  | XEN  | X axis enable.    */
    
    unsigned short RESERVED0;
    
    volatile char CTRL_REG3; /* |bit|def|   name    | info
                              * -------------------------------------------------
                              * | 7 | 0 |   DR_EN   | DRDY signal enable to INT1.
                              * | 6 | 0 |   IEA     | Interrupt signal polarity.
                              * | 5 | 0 |   IEL     | Interrupt signal latching
                              * | 4 | 0 |   INT2_EN | Interrupt 2 enable/disable
                              * | 3 | 0 |   INT1_EN | Interrupt 1 enable/disable.
                              * | 2 | 0 |   VLIFT   | Vector filter enable/disable
                              * | 0 | - |   STRT    | Soft reset bit    */
    
    volatile char CTRL_REG5; /* |bit|def|   name    | info
                              * --------------------------------------
                              * |7:6| 00|   BW2:BW1 | Anti-aliasing filter bandwidth.
                              * |5:3|000|  FSCALE2:0| Full-scale selection.
                              * |2:1| 00|   ST2:1   | Self-test enable.
                              * | 0 | 0 |   SIM     | SPI serial interface mode selection.*/
    
    volatile char CTRL_REG6; /* |bit|def|   name    | info
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
    
    char RESERVED1;
    
    volatile char STATUS;    /* READ-ONLY
                              * |bit|def|   name    | info
                              * --------------------------------------
                              * | 7 | 0 |   ZYXOR   | X, Y, and Z axis data overrun.
                              * | 6 | 0 |   ZOR     | X axis data overrun.
                              * | 5 | 0 |   YOR     | Y axis data overrun.
                              * | 4 | 0 |   XOR     | Z axis data overrun. 
                              * | 3 | 0 |   ZYXDA   | X, Y, and Z axis new data available.
                              * | 2 | 0 |   ZDA     | Z axis new data available. 
                              * | 1 | 0 |   YDA     | Y axis new data available. 
                              * | 0 | 0 |   XDA     | X axis new data available.  
                              */
    
    volatile unsigned short OUT_X_L; /* READ-ONLY; LSB X-axis output register. */
    volatile unsigned short OUT_X_H; /* READ-ONLY; MSB X-axis output register. */
    volatile unsigned short OUT_Y_L; /* READ-ONLY; LSB Y-axis output register. */
    volatile unsigned short OUT_Y_H; /* READ-ONLY; MSB Y-axis output register. */
    volatile unsigned short OUT_Z_L; /* READ-ONLY; LSB Z-axis output register. */
    volatile unsigned short OUT_Z_H; /* READ-ONLY; MSB Z-axis output register. */
    
    volatile char FIFO_CTRL; /* |bit|def | name | info
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
    
    volatile char FIFO_SRC; /* READ-ONLY
                             * |bit|   name    | info
                             * -----------------------------------------------
                             * | 7 |    WTM    | Watermark status.
                             * | 6 | OVRN_FIFO | Overrun bit status. 1: Filled
                             * | 5 |   EMPTY   | FIFO empty bit. 1: Empty
                             * |4:0|    FSS    | FIFO stored data level
                             *  -----------------------------------------------
                             * 
                             * Watermark status: 
                             * 0=FIFO filling is lower than WTM level; 
                             * 1=FIFO filling is equal or higher than WTM level */
    
} ACC_TypeDef;

#endif	/* ACC_REGISTERS_H */

