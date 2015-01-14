/* 
 * File:   spi_acc.h
 * Author: george
 *
 * Created on January 13, 2015, 8:22 PM
 */

#ifndef SPI_ACC_H
#define	SPI_ACC_H

#define SPI_ACC_FREQ        4800        //Yet to be determined
#define SPI_ACC_SLEEP_us    (int)(1000000/SPI_ACC_FREQ/16)

#define SPI_ACC_RW          0x80    /* 1: Read (OR)  0: Write (AND NOT) */
#define SPI_ACC_MS          0x60    /* 1: Master (OR) 0: Slave (AND NOT) */

void vSpiInit();
void vSpiWriteByte(char addr, char data);
void vSpiWriteShort(char addr, unsigned short data);
void vSpiWriteInt(char addr, unsigned int data);
void vSpiReadByte(char addr, char& data);
void vSpiReadBytes(char addr, char data[], int len);
void vSpiReadBytesSPIorder(char addr, char data[], int len);
void vSpiReadShort(char addr, short& data);
void vSpiReadArrayShort(char addr, short data[], int len);

#endif	/* SPI_ACC_H */

