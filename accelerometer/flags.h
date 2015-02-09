/* 
 * File:   flags.h
 * Author: george
 *
 * Created on February 8, 2015, 4:59 PM
 */

#ifndef FLAGS_H
#define	FLAGS_H

/** These definition are considered as flags to whether or not display 
 *  additional information about various parts of the project, just uncomment
 *  the parts that shouldn't display any info */
#define PROJ_DEBUG         // Display basic setup info
//#define SPI_ACC_DEBUG    // Display SPI transmission info (send/receive bytes)
#define ACC_DEBUG          // Display Accelerometer controls settings


/* DEBUGS VARIADIC MACROS */

#ifdef PROJ_DEBUG
#define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
#define DEBUG_LOG(...)
#endif

#ifdef SPI_ACC_DEBUG
#define DEBUG_SPI(...) printf(__VA_ARGS__)
#else
#define DEBUG_SPI(...)
#endif

#ifdef ACC_DEBUG
#define DEBUG_ACC(...) printf(__VA_ARGS__)
#else
#define DEBUG_ACC(...)
#endif

/***************************/



#endif	/* FLAGS_H */

