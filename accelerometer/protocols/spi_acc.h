/* 
 * File:   spi_acc.h
 * Author: george
 *
 * Created on January 13, 2015, 8:22 PM
 */

#ifndef SPI_ACC_H
#define	SPI_ACC_H

#define SPI_ACC_FREQ        2        //Yet to be determined
#define SPI_ACC_SLEEP_us    (int)(1000000/SPI_ACC_FREQ/16)

#define SPI_ACC_RW          0x80    /* 1: Read (OR)  0: Write (AND NOT) */
#define SPI_ACC_MS          0x40    /* 1: Master (OR) 0: Slave (AND NOT) */

#define SPI SPI1
#define SPI_BUFFER_RESET    0

#define SPI_ACC_CS_DELAY    1000     /* CS delay before SPI start in us */

void vSpiInit();
void vSpiWriteByte(char addr, char data);
void vSpiWriteShort(char addr, unsigned short data);
void vSpiWriteInt(char addr, unsigned int data);
char cSpiReadByte(char addr);
void vSpiReadBytes(char addr, char data[], int len);
void vSpiReadBytesSPIorder(char addr, char data[], int len);
short sSpiReadShort(char addr);
void vSpiReadArrayShort(char addr, short data[], int len);

#endif	/* SPI_ACC_H */

