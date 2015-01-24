/* 
 * File:   spi_acc.h
 * Author: george
 *
 * Created on January 13, 2015, 8:22 PM
 */

#ifndef SPI_ACC_H
#define	SPI_ACC_H

#define SPI_ACC_RW          0x80    /* 1: Read (OR)  0: Write (AND NOT) */
#define SPI_ACC_MS          0x40    /* 1: Master (OR) 0: Slave (AND NOT) */

#define SPI SPI1                    /* Set the SPI1 for SPI mems */

#define SPI_ACC_CS_DELAY    10      /* CS delay before SPI start in us */
#define SPI_ACC_WRITE_DELAY 50      /* MEMS write delay */

//#define SPI_ACC_DEBUG       /* Comment to disable transmission prints */

void vSpiInit();
void vSpiWriteByte(char addr, char data);
char cSpiReadByte(char addr);
void vSpiReadBytes(char addr, char data[], int len);
void vSpiReadBytesSPIorder(char addr, char data[], int len);
short sSpiReadShort(char addr);
void vSpiReadArrayShort(char addr, short data[], int len);

#endif	/* SPI_ACC_H */
