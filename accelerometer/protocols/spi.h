/* 
 * File:   spi.h
 * Author: george
 *
 * Created on February 8, 2015, 7:17 PM
 */

#ifndef SPI_H
#define	SPI_H

#include <miosix.h>

#include "../config.h"

// If DEBUG is enabled, prints some detailed info about SPI transmission
#ifdef SPI_ACC_DEBUG
#define DEBUG_SPI(...) printf(__VA_ARGS__)
#else
#define DEBUG_SPI(...)
#endif

#ifndef DEBUG_LOG
#define DEBUG_LOG(...)
#endif
//------------------------------------------------------------------------

#define SPI_SLAVE_CS_DELAY    10      /* CS delay before SPI start in us */
#define SPI_SLAVE_WRITE_DELAY 50      /* MEMS write delay */

#define SPI(s) reinterpret_cast<SPI_TypeDef*>(s)
using namespace miosix;

class Polarity{
public:
    enum Polarity_{
        CPHA_1_CPOL_1 = SPI_CR1_CPHA | SPI_CR1_CPOL,
        CPHA_0_CPOL_1 = SPI_CR1_CPOL,
        CPHA_0_CPOL_0 = 0x0,
        CPHA_1_CPOL_0 = SPI_CR1_CPHA
    };
private:
    Polarity();
};

class BaudRate{
public:
    enum Prescaler{
        p_2 =  0x0,
        p_4 = SPI_CR1_BR_0,
        p_8 = SPI_CR1_BR_1,
        p_16 = SPI_CR1_BR_1 | SPI_CR1_BR_0,
        p_32 = SPI_CR1_BR_2,
        p_64 = SPI_CR1_BR_2 | SPI_CR1_BR_0,
        p_128 = SPI_CR1_BR_2 | SPI_CR1_BR_1,
        p_256 = SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0
    };
private:
    BaudRate();
};

class DataFormat{
public:
    enum Format_{
        _8_bit = 0,
        _16_bit = SPI_CR1_DFF
    };
};

class Spi {
public:
    /** Create an SPI instance with the specified GPIO pins 
     * (already configured) 
     * <br> If the specified SPI is already in use then the previously created
     * <br> instance of that SPI will be returned
     * \param spi the SPI physical address
     * \param sck the already configured (MODE and AF set) GPIO pin
     * \param mosi the already configured (MODE and AF set) GPIO pin
     * \param miso the already configured (MODE and AF set) GPIO pin
     * \param cs the chip select GPIO pin  */
    static void create(unsigned int spi, Gpio sck, Gpio mosi, Gpio miso, Gpio cs);
    
    /** Write on DR register of the SPI and trigger the transmission
     * \param addr the address of the slave register where to write
     * \param byte the data (of byte size) to be transmitted to slave */
    void write(char addr, char byte);
    
    /** Write an array of bytes starting from the specified register address
     * \param addr the address of the slave register where to write
     * \param *byte the byte array to be transmitted to slave
     * \param len the length (in bytes) of the data to be written */
    void write(char addr, char *byte, int len);
    
    /** Read a byte from the specified slave register address
     * \param addr the address of the register to be read
     * \return the byte returned by slave */
    char read(char addr);
    
    /** Read an array of bytes starting from specified slave register address
     * \param addr the address of the register to start the read from
     * \param length the length of the array to be read
     * \param LSBfirst true if the receive byte order is the SPI one
     * \return the byte array read from the slave */
    char* read(char addr, unsigned int length, bool LSBfirst);
    
    /** Read a short value from the specified slave register address
     * \param addr the address of the register to be read
     * \return the unsigned short from the slave */
    unsigned short readShort(char addr);
    
    /** Read an array of unsigned short starting from the slave register address
     * <br> Note: the unsigned short values are return in standard way
     * (not SPI order)
     * \param addr the address of the register to start the read from
     * \param length the length of the array to be read
     * \return the array of unsigned short values from the slave */
    unsigned short* readShort(char addr, unsigned int length);
    
    /** Set the Baud Rate prescaler (the SPI sck frequency)
     * <br>Note: the baud rate will be set once there is no ongoing transmission
     * \param rate the BaudRate enum value */
    void setBaudRate(BaudRate rate);
    
    /** Set the Polarity (what edge to sample, and what edge to propagate)
     * <br>Note: the polarity will be set once there is no ongoing transmission
     * \param polarity the Polarity enum value */
    void setPolarity(Polarity polarity);
    
    /** Set the DataFormat (atomic data size)
     * <br>Note: the format will be set once there is no ongoing transmission
     * \param format 8bit or 16bit */
    void setDataFormat(DataFormat format);
    
    /** Full stop of the SPI (clock gating) */
    void clear();
    
    /** Full stop of the SPI (clock gating) and also destroys the SPI instance */
    static void release(unsigned int spi);
    
private:
    Gpio cs;           //The CHIP SELECT pin
    SPI_TypeDef* SPI;   //The SPI driver address
    bool transmitting;  //Helps for changing settings
    SpiModule* module;
    
    void _write(char addr, char *byte, unsigned int len);
    char* _read(char addr, unsigned int len);
    char sendReceive(char data);
    void startTransmission();
    void stopTransmission();
    
    /* Constructor: only allowed to be called inside create() method */
    Spi(SPI_TypeDef* spi, Gpio sck, Gpio mosi, Gpio miso, Gpio cs);
      
    /* List of SPI modules to keep track which modules are instantiated */
    static SpiModule* modules;
};

typedef struct SpiModule{
    SpiModule* prev;
    SpiModule* next;
    SPI_TypeDef *hwSpi;
    Spi* spi;
};
#endif	/* SPI_H */

