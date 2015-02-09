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
#define FD_TICK_TO_ms       168000

/** Number of samples of accelerometer data to be averaged */
#define FD_AVG_SAMPLES      10

/** Acceleration vector magnitude threshold, which tells whether
 *  there is a free fall or not. */
#define FD_MAG_THRESHOLD    2.0

/** Amount of time in ms that the accelerometer vector magnitude should be
 *  below threshold to be considered a free fall */
#define FD_TIME_THRESHOLD   10


/*----------------------------------------------------------------------------*/
/**** ACCELEROMETER SETTINGS -------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define ACC_G           9.8

/* Offset values: vary on each accelerometer*/
#define ACC_OFFSET_X    0x0C
#define ACC_OFFSET_Y    0x04
#define ACC_OFFSET_Z    0x0C

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

#define SPI_ACC_CONFIG      (SPI_CONFIG_BR | SPI_CONFIG_MASTER | SPI_CONFIG_POL)

#define SPI                     SPI1    /* Set the SPI1 for SPI mems */
#define SPI_3WIRE_GPIO          GPIOA_BASE
#define SPI_CS_GPIO             GPIOE_BASE
#define SPI_CS_PIN              3
#define SPI_SCK_PIN             5
#define SPI_MOSI_PIN            6
#define SPI_MISO_PIN            7
#define SPI_SCK_AF              5
#define SPI_MOSI_AF             5
#define SPI_MISO_AF             5

#define SPI_ACC_CS_DELAY        10      /* CS delay before SPI start in us */
#define SPI_ACC_WRITE_DELAY     50      /* MEMS write delay */

#define SPI_ACC_RW              0x80    /* 1: Read (OR)  0: Write (AND NOT) */
#define SPI_ACC_MS              0x40    /* 1: Master (OR) 0: Slave (AND NOT) */


/*----------------------------------------------------------------------------*/
/**** LED ALERT SETTINGS -----------------------------------------------------*/
/*----------------------------------------------------------------------------*/

#define ALERT_DELAY_GRANULARITY 20
#define ALERT_DELAY_LOADING     100
#define ALERT_DELAY_ERROR       500
#define ALERT_DELAY_FALLING     200
#define ALERT_DELAY_CRASH       400


/********************** OTHER SETTINGS ****************************************/

/** These definition are considered as flags to whether or not display 
 *  additional information about various parts of the project, just uncomment
 *  the parts that shouldn't display any info */
#define PROJ_DEBUG         // Display basic setup info
//#define SPI_ACC_DEBUG    // Display SPI transmission info (send/receive bytes)
#define ACC_DEBUG          // Display Accelerometer controls settings

/****************** DEBUGS VARIADIC MACROS ************************************/

#ifdef PROJ_DEBUG
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#ifdef ACC_DEBUG
#define DEBUG_ACC(...) printf(__VA_ARGS__)
#else
#define DEBUG_ACC(...)
#endif

// If DEBUG is enabled, prints some detailed info about SPI transmission
#ifdef SPI_ACC_DEBUG
#define DEBUG_SPI(...) printf(__VA_ARGS__)
#else
#define DEBUG_SPI(...)
#endif

/******************************************************************************/

#endif	/* FLAGS_H */