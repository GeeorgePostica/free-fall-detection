/* 
 * File:   spi.cpp
 * Author: george
 * 
 * Created on February 8, 2015, 7:17 PM
 */
#include "spi.h"
#include <stdio.h>
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

char SpiBase::read(char addr) {
    DEBUG_SPI("Reading from reg: 0x%02x ...", addr);
    char byte[1];
    return _read(addr, byte, 1)[0];
}

char* SpiBase::read(char addr, char data[], unsigned int length, bool LSBfirst) {
    if (LSBfirst) {
        DEBUG_SPI("Reading spi order bytes, start reg: 0x%02x ...", addr);
        return _read(addr, data, length);
    }

    char *bytes;

    DEBUG_SPI("Reading bytes, start reg: 0x%02x ...", addr);

    bytes = _read(addr, data, length);
    for (unsigned int i = 0; i < length; i++) {
        data[i] = bytes[length - i - 1];
    }
    return data;
}

short SpiBase::readShort(char addr) {
    char bytes[2];
    DEBUG_SPI("Reading short from reg: 0x%02x ...", addr);
    _read(addr, bytes, 2);
    return (unsigned short) ((bytes[1] << 8) | bytes[0]);
}

short* SpiBase::readShort(char addr, short data[], unsigned int length) {
    char bytes[length * 2];
    DEBUG_SPI("Reading shorts, start reg: 0x%02x ...", addr);
    _read(addr, bytes, length * 2);
    for (unsigned int i = 0; i < length; i++) {
        data[i] = (short) ((bytes[2 * i + 1] << 8) | bytes[2 * i]);
    }
    return data;
}

char* SpiBase::_read(char addr, char data[], unsigned int len) {
    /* Basic control over the length of data to be read*/
    if (len < 1) return 0;

    /* Start the accelerometer SPI */
    startTransmission();

    DEBUG_SPI("\n   sent byte: \t0x%02x\n", addr);

    /* Send the register address to be written to*/
    sendReceive(addr);

    DEBUG_SPI("   received:");

    /* Receiving the (multiple) data bytes in SPI order */
    for (unsigned int i = 0; i < len; i++) {
        /* Send dummy data and received in SPI mode that is:
         *  from the LS byte to the MS byte */
        data[i] = sendReceive(0x00);

        DEBUG_SPI("\t0x%02x", data[i]);
    }

    DEBUG_SPI("\n");

    /* Stop the slave SPI */
    stopTransmission();

    return data;
}

void SpiBase::write(char addr, char byte) {
    DEBUG_SPI("\nWriting to reg:0x%02x byte:0x%02x", addr, data);
    _write(addr, &byte, 1);
}

void SpiBase::write(char addr, char* byte, unsigned int len) {
    DEBUG_SPI("\nWriting to reg:0x%02x", addr);
    _write(addr, byte, len);
}

void SpiBase::_write(char addr, char* byte, unsigned int len) {
    /* Start the SPI transmission */
    startTransmission();

    DEBUG_SPI("\n   sent byte: \t0x%02x", addr);

    /* Send the register address to be written to*/
    sendReceive(addr);

    unsigned int i = 0;
    while (i < len) {
        /* Sending the data to be written */
        DEBUG_SPI("\t0x%02x", byte[i]);
        sendReceive(byte[i++]);
    }

    /* Stop the accelerometer SPI */
    stopTransmission();

    DEBUG_SPI("\n");
}