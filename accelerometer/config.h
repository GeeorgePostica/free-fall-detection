/* 
 * File:   flags.h
 * Author: george
 *
 * Created on February 8, 2015, 4:59 PM
 */

#ifndef CONFIG_H
#define	CONFIG_H

/** These definition are considered as flags to whether or not display 
 *  additional information about various parts of the project, just uncomment
 *  the parts that shouldn't display any info */
#define PROJ_DEBUG         // Display basic setup info
//#define SPI_ACC_DEBUG    // Display SPI transmission info (send/receive bytes)
#define ACC_DEBUG          // Display Accelerometer controls settings


/*------ General configuration parameters -----------------*/
#define SPI_ACC_RW          0x80    /* 1: Read (OR)  0: Write (AND NOT) */
#define SPI_ACC_MS          0x40    /* 1: Master (OR) 0: Slave (AND NOT) */


/*---------------------------------------------------------*/

/* DEBUGS VARIADIC MACROS */

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

/***************************/



#endif	/* FLAGS_H */

