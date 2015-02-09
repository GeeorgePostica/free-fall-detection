/* 
 * File:   spi.cpp
 * Author: george
 * 
 * Created on February 8, 2015, 7:17 PM
 */
#include "spi.h"
#include <miosix.h>
#include <stdio.h>

#include "../config.h"

#ifndef DEBUG_LOG
#define DEBUG_LOG(...)
#endif
//------------------------------------------------------------------------

#define SPI_SLAVE_CS_DELAY    10      /* CS delay before SPI start in us */
#define SPI_SLAVE_WRITE_DELAY 50      /* MEMS write delay */

#define SPI SPI1
using namespace miosix;

/* GPIO pins declarations */
typedef Gpio<GPIOA_BASE, 5> sck;
typedef Gpio<GPIOA_BASE, 6> miso;
typedef Gpio<GPIOA_BASE, 7> mosi;
typedef Gpio<GPIOE_BASE, 3> cs;

void Spi::init() {
    DEBUG_LOG("\nSPI initializing... ");
    
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN; /* Enable clock for SPI */
    
    /* Set the alternate functions */
    sck::mode(Mode::ALTERNATE);
    sck::alternateFunction(5);
    miso::mode(Mode::ALTERNATE);
    miso::alternateFunction(5);
    mosi::mode(Mode::ALTERNATE);
    mosi::alternateFunction(5);

    /* Set the slave select pin in output mode*/
    cs::mode(Mode::OUTPUT);
    cs::high();

    /* Basic SPI configuration for LIS3DSH accelerometer
     * MSB first; 8bit data format; CPHA=1 CPOL=1; */

    SPI->CR1 |= SPI_CR1_BR_2; /* Set the baud pre-scaler to 32 (5.25MHz) */
    SPI->CR1 |= SPI_CR1_CPHA; /* Set the CPHA=1 */
    SPI->CR1 |= SPI_CR1_CPOL; /* Set the CPOL=1 */
    SPI->CR1 |= SPI_CR1_MSTR; /* Set the master mode */
    SPI->CR1 |= SPI_CR1_SSM; /* Set software slave select */
    SPI->CR1 |= SPI_CR1_SSI; /* Set internal slave select */


    SPI->CR1 |= SPI_CR1_SPE; /* Enable the SPI */

    DEBUG_LOG("done: CR1 = 0x%08x\n\n", (unsigned int) (SPI->CR1));
}

char Spi::sendReceive(char data) {
    /* Wait for TX line to be empty to avoid overwriting data*/
    while (!(SPI->SR & SPI_SR_TXE));

    /* Write the data to SPI data register, this triggers the
     * the SCK clocking and transmission to start */
    SPI->DR = data;

    /* Wait for RX line to be filled and return the data from RX buffer*/
    while (!(SPI->SR & SPI_SR_RXNE));

    return SPI->DR;
}

void Spi::startTransmission() {
    /* Pull down slave select, which enables the accelerometer SPI */
    cs::low();

    /* Let the MEMS catch the CS pull down */
    delayUs(SPI_SLAVE_CS_DELAY);

    /* Enable the SPI */
    SPI->CR1 |= SPI_CR1_SPE;
}

void Spi::stopTransmission() {
    /* Wait until TX buffer empty, this ensures no communication is ongoing */
    while (!(SPI->SR & SPI_SR_TXE));

    /* Wait until SPI is not busy */
    while (SPI->SR & SPI_SR_BSY);

    /* Let the MEMS catch the SCK stable high value */
    delayUs(SPI_SLAVE_CS_DELAY);

    /* Disable the SPI */
    SPI->CR1 &= ~SPI_CR1_SPE;

    /* Pull up slave select, which disables the accelerometer SPI */
    cs::high();

    /* Let the MEMS catch the CS pull up */
    delayUs(SPI_SLAVE_CS_DELAY);
}

void Spi::stop() {
    while(SPI->CR1 & SPI_CR1_SPE);
    RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
}