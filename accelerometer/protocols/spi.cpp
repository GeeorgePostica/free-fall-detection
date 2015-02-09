/* 
 * File:   spi.cpp
 * Author: george
 * 
 * Created on February 8, 2015, 7:17 PM
 */
#include "spi.h"

using namespace miosix;

Spi::Spi(SPI_TypeDef* spi, Gpio sck, Gpio mosi, Gpio miso, Gpio cs) {
    DEBUG_LOG("\nSPI initializing... ");

    /* Enable clock for SPI */
    switch (spi) {
        case SPI1:
            RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;
            break;
        case SPI2:
            RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
            break;
        default:
            RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
            break;
    }

    /* Set the slave select pin in output mode*/
    cs::mode(Mode::OUTPUT);
    cs::high();
    this->cs = cs;

    this->SPI = spi;
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

void Spi::create(unsigned int spi, Gpio sck, Gpio mosi, Gpio miso, Gpio cs) {
    SpiModule* iter = modules;
    while (iter != NULL) {
        if (iter->hwSpi == SPI(spi)) {
            return iter->spi;
        }
        iter = iter->next;
    }
    SpiModule* newElem = (SpiModule*) malloc(sizeof (SpiModule));
    newElem->hwSpi = SPI(spi);
    newElem->spi = new Spi(spi, sck, mosi, miso, cs);
    if (iter->prev == NULL) { //First element
        modules = newElem;
    } else {
        iter->prev->next = newElem;
        newElem->prev = iter->prev;
    }
    newElem->spi->module = newElem;
    return newElem->spi;
}

char Spi::read(char addr) {
    DEBUG_SPI("Reading from reg: 0x%02x ...", addr);
    return _read(addr, 1)[0];
}

char* Spi::read(char addr, unsigned int length, bool LSBfirst) {
    if (LSBfirst) {
        DEBUG_SPI("Reading spi order bytes, start reg: 0x%02x ...", addr);
        return _read(addr, length);
    }
    
    char *bytes;
    char data[length];
    
    DEBUG_SPI("Reading bytes, start reg: 0x%02x ...", addr);
    
    bytes = _read(addr, length);
    for(int i=0; i<length; i++){
        data[i] = bytes[length - i - 1];
    }
    return data;
}

unsigned short Spi::readShort(char addr) {
    char *bytes;
    DEBUG_SPI("Reading short from reg: 0x%02x ...", addr);
    bytes = _read(addr, 2);
    return (unsigned short) ((bytes[1] << 8) | bytes[0]);
}

unsigned short* Spi::readShort(char addr, unsigned int length) {
    char *bytes;
    unsigned short data[length];
    DEBUG_SPI("Reading shorts, start reg: 0x%02x ...", addr);
    bytes = _read(addr, length*2);
    for(int i=0; i< length; i++){
        data[i] = (unsigned short) ((bytes[2*i+1] << 8) | bytes[2*i]);
    }
    return data;
}

char* Spi::_read(char addr, unsigned int len) {
    /* Basic control over the length of data to be read*/
    if(len < 1) return 0;
    
    char data[len];
    
    /* Start the accelerometer SPI */
    startTransmission();
    
    DEBUG_SPI("\n   sent byte: \t0x%02x\n", addr);
    
    /* Send the register address to be written to*/
    sendReceive(addr);
    
    DEBUG_SPI("   received:");
    
    /* Receiving the (multiple) data bytes in SPI order */
    for(int i=0; i<len; i++){
        /* Send dummy data and received in SPI mode that is:
         *  from the LS byte to the MS byte */
        data[i] = sendReceive(0x00);  
        
        DEBUG_SPI("\t0x%02x", data[i]);
    }
    
    DEBUG_SPI("\n");
    
    /* Stop the slave SPI */
    stopTransmission();
}


void Spi::write(char addr, char byte) {
    DEBUG_SPI("\nWriting to reg:0x%02x byte:0x%02x", addr, data);
    _write(addr, &byte, 1);
}

void Spi::write(char addr, char* byte, unsigned int len) {
    DEBUG_SPI("\nWriting to reg:0x%02x", addr);
    _write(addr, byte, len);
}

void Spi::_write(char addr, char* byte, unsigned int len) {
    /* Start the SPI transmission */
    startTransmission();

    DEBUG_SPI("\n   sent byte: \t0x%02x", addr);
    
    /* Send the register address to be written to*/
    sendReceive(addr);

    int i = 0;
    while(i < len){
        /* Sending the data to be written */
        DEBUG_SPI("\t0x%02x", byte[i]);
        sendReceive(byte[i++]);
    }
    
    /* Stop the accelerometer SPI */
    stopTransmission();
    
    DEBUG_SPI("\n");
    
    /* Leave the slave time to write the registers*/
    delayUs(SPI_SLAVE_WRITE_DELAY);
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
    //Wait for the SPI to be free
    while (transmitting) {
        delayUs(10);
    }
    
    /* Pull down slave select, which enables the accelerometer SPI */
    cs::low();

    /* Set the transmitting boolean */
    transmitting = true;

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

    /* Clear the transmitting boolean */
    transmitting = false;

    /* Pull up slave select, which disables the accelerometer SPI */
    cs::high();

    /* Let the MEMS catch the CS pull up */
    delayUs(SPI_SLAVE_CS_DELAY);
}

void Spi::setBaudRate(BaudRate rate) {
    //Wait for the SPI to be free
    while (transmitting) {
        delayUs(10);
    };
    SPI->CR1 = (SPI->CR1 & ~(7 << 3)) | rate;
}

void Spi::setDataFormat(DataFormat format) {
    //Wait for the SPI to be free
    while (transmitting) {
        delayUs(10);
    }
    SPI->CR1 = (SPI->CR1 & ~(1 << 11)) | format;
}

void Spi::setPolarity(Polarity polarity) {
    //Wait for the SPI to be free
    while (transmitting) {
        delayUs(10);
    }
    SPI->CR1 = (SPI->CR1 & ~(2)) | polarity;
}

void Spi::clear() {
    //Wait for the SPI to be free
    while (transmitting) {
        delayUs(10);
    }
    /* Disable clock for SPI */
    switch (SPI) {
        case SPI1:
            RCC->APB2ENR &= ~RCC_APB2ENR_SPI1EN;
            break;
        case SPI2:
            RCC->APB1ENR &= ~RCC_APB1ENR_SPI2EN;
            break;
        default:
            RCC->APB1ENR &= ~RCC_APB1ENR_SPI3EN;
            break;
    }
    if(module->prev != NULL && module->next != NULL){
        module->prev->next = module->next;
    }
    else if(module->prev != NULL){
        module->prev->next = NULL;
    }
    free(module);
}

void Spi::release(unsigned int spi) {
    if(modules->hwSpi == SPI(spi) && modules->next == NULL){
        free(modules);
    }
    SpiModule* iter = modules;
    while (iter != NULL) {
        if (iter->hwSpi == SPI(spi)) {
            iter->spi->clear();
            return;
        }
        iter = iter->next;
    }
}