/* 
 * File:   acc_registers.h
 * Author: george
 * This file contains all the needed register addresses and other 
 * register controls for LIS3DSH accelerometer on 
 * STM32F407Discovery board
 * Created on January 13, 2015, 1:21 PM
 */

#ifndef ACC_REGISTERS_H
#define	ACC_REGISTERS_H

/* Accelerometer Operating Data Rates*/
#define ACC_ODR_MASK        0xF0
#define ACC_ODR_POWER_DOWN  0x00
#define ACC_ODR_3125mHz     0x10
#define ACC_ODR_6250mHz     0x20
#define ACC_ODR_12500mHz    0x30
#define ACC_ODR_25Hz        0x40
#define ACC_ODR_50Hz        0x50
#define ACC_ODR_100Hz       0x60
#define ACC_ODR_400Hz       0x70
#define ACC_ODR_800Hz       0x80
#define ACC_ODR_1600Hz      0x90

/* Accelerometer Full-Scale Values*/
#define ACC_FSCALE_MASK     0x38
#define ACC_FSCALE_2G       0x00
#define ACC_FSCALE_4G       0x08    /* 4G Scale */
#define ACC_FSCALE_6G       0x10
#define ACC_FSCALE_8G       0x18
#define ACC_FSCALE_16G      0x20

/* Self Test Selection */
#define ACC_SELF_TEST_MASK              0x06
#define ACC_SELF_TEST_NORMAL            0x00
#define ACC_SELF_TEST_POSITIVE_SIGN     0x02
#define ACC_SELF_TEST_NEGATIVE_SIGN     0x04
#define ACC_SELF_TEST_PROHIBITED        0x06

/* FIFO Selection modes */
#define ACC_FIFO_MODE_MASK                  0xE0
#define ACC_FIFO_MODE_BYPASS                0x00
#define ACC_FIFO_MODE_FIFO                  0x20
#define ACC_FIFO_MODE_STREAM                0x40
#define ACC_FIFO_MODE_STREAM_THEN_FIFO      0x60
#define ACC_FIFO_MODE_BYPASS_THEN_STREAM    0x80
#define ACC_FIFO_MODE_BYPASS_THEN_FIFO      0xE0

/* Accelerometer Axis for enabling/disabling */
#define ACC_AXIS_MASK   0x05
#define ACC_AXIS_X      0x01
#define ACC_AXIS_Y      0x02
#define ACC_AXIS_Z      0x04

/* SPI Selection Modes */
#define ACC_SPI_MODE_3WIRE  0x01
#define ACC_SPI_MODE_4WIRE  0x00

/******************** REGISTERS ADDRESSES **************************/
//#define ACC ((ACC_TypeDef *) 0x20)

#define ACC_ADDR_CTRL_REG3  0x23    /*ADDRESS: 0x23
                                    * |bit|def|   name    | info
                                    * -------------------------------------------------
                                    * | 7 | 0 |   DR_EN   | DRDY signal enable to INT1.
                                    * | 6 | 0 |   IEA     | Interrupt signal polarity.
                                    * | 5 | 0 |   IEL     | Interrupt signal latching
                                    * | 4 | 0 |   INT2_EN | Interrupt 2 enable/disable
                                    * | 3 | 0 |   INT1_EN | Interrupt 1 enable/disable.
                                    * | 2 | 0 |   VLIFT   | Vector filter enable/disable
                                    * | 0 | - |   STRT    | Soft reset bit    */

#define ACC_ADDR_CTRL_REG4  0x20   /* ADDRESS: 0x20
                                    * |bit|def | name | info
                                    * -----------------------------------------------
                                    * |7:4|0000| ODR  | Operating data and power rate
                                    * | 3 | 0  | BDU  | Block data update.
                                    * | 2 | 1  | ZEN  | Z axis enable.
                                    * | 1 | 1  | YEN  | Y axis enable.
                                    * | 0 | 1  | XEN  | X axis enable.    */

#define ACC_ADDR_CTRL_REG5  0x24    /*ADDRESS: 0x24
                                    * |bit|def|   name    | info
                                    * --------------------------------------
                                    * |7:6| 00|   BW2:BW1 | Anti-aliasing filter bandwidth.
                                    * |5:3|000|  FSCALE2:0| Full-scale selection.
                                    * |2:1| 00|   ST2:1   | Self-test enable.
                                    * | 0 | 0 |   SIM     | SPI serial interface mode selection.*/

#define ACC_ADDR_CTRL_REG6  0x25    /*ADDRESS: 0x25
                                    * |bit|def|   name    | info
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

#define ACC_ADDR_STATUS     0x27    /*ADDRESS: 0x27
                                    * READ-ONLY
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

#define ACC_ADDR_FIFO_CTRL  0x2E    /*ADDRESS: 0x2E
                                    * |bit|def | name | info
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

#define ACC_ADDR_FIFO_SRC   0x2F    /*ADDRESS: 0x2F
                                    * READ-ONLY
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

#define ACC_ADDR_OUT_X_L    0x28    /* ADDRESS: 0x28; READ-ONLY; LSB X-axis output register. */
#define ACC_ADDR_OUT_X_H    0x29    /* ADDRESS: 0x29; READ-ONLY; MSB X-axis output register. */
#define ACC_ADDR_OUT_Y_L    0x2A    /* ADDRESS: 0x2A; READ-ONLY; LSB Y-axis output register. */
#define ACC_ADDR_OUT_Y_H    0x2B    /* ADDRESS: 0x2B; READ-ONLY; MSB Y-axis output register. */
#define ACC_ADDR_OUT_Z_H    0x2C    /* ADDRESS: 0x2C; READ-ONLY; LSB Z-axis output register. */
#define ACC_ADDR_OUT_Z_L    0x2D    /* ADDRESS: 0x2D; READ-ONLY; MSB Z-axis output register. */

/*******************************************************************************/


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
    
    char STATUS;             /* READ-ONLY
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
    
    short OUT_X;    /* READ-ONLY; X-axis output register. */
    short OUT_Y;    /* READ-ONLY; Y-axis output register. */
    short OUT_Z;    /* READ-ONLY; Z-axis output register. */
    
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
    
    char FIFO_SRC;          /* READ-ONLY
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

/************ Accelerometer registers control functions **********************/
//#define LOCAL_ACC_FUNCTIONS
#ifndef LOCAL_ACC_FUNCTIONS        /* Define it to have different set of functions*/

ACC_TypeDef ACC;

void vAccRegSetODR(const char ODR);

void vAccRegSetFScale(const char FScale);

void vAccRegSelfTest(const char SelfTest);

void vAccRegSetFIFOMode(const char FIFOMode);

void vAccRegBlockDataUpdate();

void vAccRegEnableDataUpdate();

void vAccRegSoftReset();

void vAccRegSetSPIMode(const char SPIMode);

void vAccRegEnableAxis(const char Axis);

void vAccRegDisableAxis(const char Axis);

void vAccRegEnableFIFO();

void vAccRegDisableFIFO();

void vAccRegForceReboot();

void vAccRegAddrIncON();

void vAccRegAddrIncOFF();

int iAccRegIsDataOverrun();

int iAccRegIsDataOverrunAxis(const char Axis);

int iAccRegIsDataReady();

int iAccRegIsDataReadyAxis(const char Axis);

int iAccRegIsFIFOFilled();

int iAccRegIsFIFOEmpty();

#else
void vAccRegSetODR(ACC_TypeDef &ACC, const char ODR);

void vAccRegSetFScale(ACC_TypeDef &ACC, const char FScale);

void vAccRegSelfTest(ACC_TypeDef &ACC, const char SelfTest);

void vAccRegSetFIFOMode(ACC_TypeDef &ACC, const char FIFOMode);

void vAccRegBlockDataUpdate(ACC_TypeDef &ACC);

void vAccRegEnableAxis(ACC_TypeDef &ACC, const char Axis);

void vAccRegDisableAxis(ACC_TypeDef &ACC, const char Axis);

void vAccRegSoftReset(ACC_TypeDef &ACC);

void vAccRegSetSPIMode(ACC_TypeDef &ACC, const char SPIMode);

void vAccRegEnableFIFO(ACC_TypeDef &ACC);

void vAccRegDisableFIFO(ACC_TypeDef &ACC);

void vAccRegForceReboot(ACC_TypeDef &ACC);

void vAccRegAddrIncON(ACC_TypeDef &ACC);

void vAccRegAddrIncOFF(ACC_TypeDef &ACC);

int iAccRegIsDataOverrun(ACC_TypeDef ACC);

int iAccRegIsDataOverrun(ACC_TypeDef &ACC, const char Axis);

int iAccRegIsDataReady(ACC_TypeDef ACC);

int iAccRegIsDataReady(ACC_TypeDef &ACC, const char Axis);

int iAccRegIsFIFOFilled(ACC_TypeDef ACC);

int iAccRegIsFIFOEmpty(ACC_TypeDef ACC);

#endif


#endif	/* ACC_REGISTERS_H */

