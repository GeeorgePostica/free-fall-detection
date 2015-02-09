/* 
 * File:   spi.h
 * Author: george
 *
 * Created on February 8, 2015, 7:17 PM
 */

#ifndef SPI_H
#define	SPI_H

/*-------------- RELATIVE TO ALL SPIs -----------------*/
struct SpiModule;

class SpiBase {
public:
    /** Write on DR register of the SPI and trigger the transmission
     * \param addr the address of the slave register where to write
     * \param byte the data (of byte size) to be transmitted to slave */
    static void write(char addr, char byte);

    /** Write an array of bytes starting from the specified register address
     * \param addr the address of the slave register where to write
     * \param *byte the byte array to be transmitted to slave
     * \param len the length (in bytes) of the data to be written */
    static void write(char addr, char *byte, unsigned int len);

    /** Read a byte from the specified slave register address
     * \param addr the address of the register to be read
     * \return the byte returned by slave */
    static char read(char addr);

    /** Read an array of bytes starting from specified slave register address
     * \param addr the address of the register to start the read from
     * \param length the length of the array to be read
     * \param LSBfirst true if the receive byte order is the SPI one
     * \return the byte array read from the slave */
    static char* read(char addr, char *data, unsigned int length, bool LSBfirst);

    /** Read a short value from the specified slave register address
     * \param addr the address of the register to be read
     * \return the unsigned short from the slave */
    static short readShort(char addr);

    /** Read an array of unsigned short starting from the slave register address
     * <br> Note: the unsigned short values are return in standard way
     * (not SPI order)
     * \param addr the address of the register to start the read from
     * \param length the length of the array to be read
     * \return the array of unsigned short values from the slave */
    static short* readShort(char addr, short *data, unsigned int length);

protected:
    static char sendReceive(char data);
    static void startTransmission();
    static void stopTransmission();
    static void _write(char addr, char *byte, unsigned int len);
    static char* _read(char addr, char *data, unsigned int len);
};

typedef struct SpiModule {
    unsigned int *sckGpioAddress;
    unsigned char sckPortNumber;
    unsigned char sckAF;
    unsigned int *mosiGpioAddress;
    unsigned char mosiPortNumber;
    unsigned char mosiAF;
    unsigned int *misoGpioAddress;
    unsigned char misoPortNumber;
    unsigned char misoAF;
    
    unsigned int REG1;  //Content of first control register
    unsigned int REG2;  //Content of second control register
    
    int initialized;
} SpiModule;

class Spi: public SpiBase{
public:
    /** Create an SPI instance 
     * <br> If the specified SPI is already in use then the previously created
     * <br> instance of that SPI will be returned
     * \param spi the SPI driver physical address
     * \return 1 if the SPI initialized, 0 if it was previously initialized */
    static void init();
    
    /** Full stop of the SPI (clock gating) */
    static void stop();
    
private:
    static char sendReceive(char data);
    static void startTransmission();
    static void stopTransmission();
};

#endif	/* SPI_H */

