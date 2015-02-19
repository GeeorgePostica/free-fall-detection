/* 
 * File:   flags.h
 * Author: george
 *
 * Created on February 8, 2015, 4:59 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

#include <stdio.h>

/*************** General configuration parameters *****************************/

/*----------------------------------------------------------------------------*/
/**** FREE FALL SETTINGS -----------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/** Timer tick divider to get 1ms */
//#define FD_TICK_TO_ms       168000  // Timer->interval() returns ms not ticks!!

/** Number of samples of accelerometer data to be averaged */
#define FD_AVG_SAMPLES      2

/** Acceleration vector magnitude threshold, which tells whether
 *  there is a free fall or not. */
#define FD_MAG_THRESHOLD    2.5

/** Amount of time in ms that the accelerometer vector magnitude should be
 *  below threshold to be considered a free fall */
#define FD_TIME_THRESHOLD   80

/** Amount of samples for the crash analysis algorithm to get from MEMS */
#define FD_CRASH_ANALYSIS_SAMPLES 1

/** Acceleration vector magnitude threshold, which tells whether
 *  there is still a free fall or not. */
#define FD_CRASH_MAG_THRESHOLD    4.0

/** Board weight [kg]*/
#define FD_CRASH_BOARD_MASS       0.06

/*----------------------------------------------------------------------------*/
/**** ACCELEROMETER SETTINGS -------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define ACC_G           9.8

/* Offset values: vary on each accelerometer*/
#define ACC_OFFSET_X    0x00    //0x0C
#define ACC_OFFSET_Y    0x00    //0x04
#define ACC_OFFSET_Z    0x00    //0x0C

/* Initial values for accelerometer registers */
#define ACC_INIT_REG3   0x00;  // Data ready disabled, interrupts disabled
#define ACC_INIT_REG4   0x87;  // 800Hz ODR, x/y/z enabled
#define ACC_INIT_REG5   0x00;  // 800Hz AA, no self-test, 4-wire SPI
#define ACC_INIT_REG6   0x10;  // FIFO disabled, Auto-increment enabled
#define ACC_INIT_FIFO   0x00;  // FIFO turned off

/*----------------------------------------------------------------------------*/
/**** SPI SETTINGS -----------------------------------------------------------*/
/*----------------------------------------------------------------------------*/

/* Basic SPI configuration for LIS3DSH accelerometer
 * MSB first; 8bit data format; CPHA=1 CPOL=1; */
#define SPI_CONFIG_BR           SPI_CR1_BR_2   /*Baud pre-scaler: 32 (5.25MHz)*/
#define SPI_CONFIG_POL         (SPI_CR1_CPHA | SPI_CR1_CPOL) /* CPHA=1 CPOL=1 */
/* Configure in master mode and software and internal slave selection */
#define SPI_CONFIG_MASTER       (SPI_CR1_MSTR | SPI_CR1_SSM | SPI_CR1_SSI)

#define SPI_CONFIG_REG      (SPI_CONFIG_BR | SPI_CONFIG_MASTER | SPI_CONFIG_POL)

#define SPI                     SPI1    /* Set the SPI1 for SPI mems */
#define SPI_SCK_MOSI_MISO_GPIO  GPIOA_BASE
#define SPI_CS_GPIO             GPIOE_BASE
#define SPI_CS_PIN              3
#define SPI_SCK_PIN             5
#define SPI_MOSI_PIN            6
#define SPI_MISO_PIN            7
#define SPI_SCK_AF              5
#define SPI_MOSI_AF             5
#define SPI_MISO_AF             5

#define SPI_ACC_CS_DELAY        5      /* CS delay before SPI start in us */
#define SPI_ACC_WRITE_DELAY     10      /* MEMS write delay in us */

#define SPI_ACC_RW              0x80    /* 1: Read (OR)  0: Write (AND NOT) */
#define SPI_ACC_MS              0x40    /* 1: Master (OR) 0: Slave (AND NOT) */


/****************** DEBUGS VARIADIC MACROS ************************************/
/** These definition are variadic macros which display any 
 *  additional information about various parts of the project, just uncomment
 *  the parts that shouldn't display any info */

// Display basic setup info
#define DEBUG_LOG(...) iprintf(__VA_ARGS__)

/********************** OTHER SETTINGS ****************************************/
/* These flags are used in case there are some uncommented debug macros above*/
#ifndef DEBUG_LOG
#define DEBUG_LOG(...)
#endif
/******************************************************************************/

#endif	/* FLAGS_H */