/* 
 * File:   spi_acc.h
 * Author: george
 *
 * Created on January 13, 2015, 8:22 PM
 */

#ifndef SPI_ACC_H
#define	SPI_ACC_H

#include "../config.h"

#ifndef DEBUG_LOG
#define DEBUG_LOG(...)
#endif
//------------------------------------------------------------------------
/**
 * Initiates the SPI and configures it */
void vSpiInit();

/**
 * Send a byte through SPI
 * @param addr the register address where to write
 * @param data the data byte to be sent */
void vSpiWriteByte(char addr, char data);

/**
 * Receive a data byte from specified slave register
 * @param addr the slave register address where to read from
 * @return the data byte received from slave */
char cSpiReadByte(char addr);

/**
 * Receive a short value from the slave over the SPI
 * @param addr the address of the register where to read from
 * @return the short value from the slave */
short sSpiReadShort(char addr);

/**
 * Receive an array of short values over the SPI
 * @param addr the address of the register where to start the read from
 * @param data the array where to store the received values
 * @param len the length of the array */
void vSpiReadArrayShort(char addr, short data[], int len);

/**
 * Stops the SPI (clock gating) */
void vSpiShutdown();
#endif	/* SPI_ACC_H */

