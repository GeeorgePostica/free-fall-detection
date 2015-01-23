#include <miosix.h>
#include <stdio.h>
#include "spi_acc.h"

using namespace miosix;

typedef Gpio<GPIOA_BASE,5> sck;
typedef Gpio<GPIOA_BASE,6> miso;
typedef Gpio<GPIOA_BASE,7> mosi;
typedef Gpio<GPIOE_BASE,3> cs;

/***************FUNCTION PROTOTYPES************/
int iSpiRead(char addr, char* data, int len);
void vSpiStartTransmission();
void vSpiEndTransmission();
char cSpiSendReceiveData(char data);
/**********************************************/

void vSpiInit(){
    
    
    RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;                 /* Enable clock for SPI */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;                /* Enable clock on GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;                /* Enable clock on GPIOA */
    
    sck::mode(Mode::ALTERNATE);
    sck::alternateFunction(5);
    miso::mode(Mode::ALTERNATE);
    miso::alternateFunction(5);
    mosi::mode(Mode::ALTERNATE);
    mosi::alternateFunction(5);
    
    cs::mode(Mode::OUTPUT);
    cs::high();
    
    /* Basic SPI configuration for LIS3DSH accelerometer
     * MSB first; 8bit data format; CPHA=1 CPOL=1; */
    SPI->CR1 |= SPI_CR1_BR_2;                       /* Set the baud pre multiplier to 32 (5.25MHz) */
    SPI->CR1 |= SPI_CR1_CPHA;                       /* Set the CPHA=1 */
    SPI->CR1 |= SPI_CR1_CPOL;                       /* Set the CPOL=1 */
    SPI->CR1 |= SPI_CR1_MSTR;                       /* Set the master mode */
    SPI->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;          /* Manage software slave select */
    
    
    SPI->CR1 |= SPI_CR1_SPE;                        /* Enable the SPI */
    
    printf("\n SPI init status:\n SPI->CR1: 0x%08x\n\n", (unsigned int)(SPI->CR1));
}

void vSpiWriteByte(char addr, char data){
    printf("\nWriting to reg:0x%02x byte:0x%02x", addr, data);
    
    /* Start the accelerometer SPI */
    vSpiStartTransmission();
    
    printf("\n   sent byte: \t0x%02x\n", addr);
    
    /* Send the register address to be written to*/
    cSpiSendReceiveData(addr);
    
    /* Sending the data to be written */
    cSpiSendReceiveData(data);
    
    /* Stop the accelerometer SPI */
    vSpiEndTransmission();
}

char cSpiReadByte(char addr){
    char byte[] = { 0 };
    printf("Reading from reg: 0x%02x ...", addr);
    iSpiRead(addr, byte, 1);
    return byte[0];
}

void vSpiReadBytesSPIorder(char addr, char data[], int len){
    iSpiRead(addr, data, len);
}

void vSpiReadBytes(char addr, char data[], int len){
    char bytes[len];
    iSpiRead(addr, bytes, len);
    for(int i=0; i<len; i++){
        data[i] = bytes[len - i - 1];
    }
}

short sSpiReadShort(char addr){
    char bytes[2];
    iSpiRead(addr, bytes, 2);
    return (short) (bytes[1]*0x100 + bytes[0]);
}

void vSpiReadArrayShort(char addr, short data[], int len){
    char bytes[2*len];
    iSpiRead(addr, bytes, 2*len);
    for(int i=0; i< len; i++){
        data[i] = bytes[2*i+1]*0x100 + bytes[2*i];
    }
}

int iSpiRead(char addr, char* data, int len){
    /* Basic control over the length of data to be read*/
    if(len < 1) return 0;
    
    /* Set the address in read mode */
    char byte1 = (addr | SPI_ACC_RW);
    
    /* Start the accelerometer SPI */
    vSpiStartTransmission();
    
    printf("\n   sent byte: \t0x%02x\n", byte1);
    
    /* Send the register address to be written to*/
    byte1 = cSpiSendReceiveData(byte1);
    
    printf("   received: \t0x%02x\t", byte1);
    
    /* Receiving the (multiple) data bytes in SPI order */
    for(int i=0; i<len; i++){
        /* Send dummy data and received in SPI mode that is:
         *  from the LS byte to the MS byte */
        data[i] = cSpiSendReceiveData(0x00);  
        
        printf("0x%02x\t", data[i]);
    }
    
    printf("\n");
    
    /* Stop the accelerometer SPI */
    vSpiEndTransmission();
    return 1;
}

char cSpiSendReceiveData(char data){
    /* Wait for TX line to be empty to avoid overwriting data*/
    while(!(SPI->SR & SPI_SR_TXE));
    
    /* Write the data to SPI data register, this triggers the
     * the SCK clocking and transmission to start */
    SPI->DR = data;
    
    /* Wait for RX line to be filled and return the data from RX buffer*/
    while(!(SPI->SR & SPI_SR_RXNE));
    
    return SPI->DR;
}

void vSpiStartTransmission(){
    /* Pull down slave select, which enables the accelerometer SPI */
    cs::low();
    
    /* Let the MEMS catch the CS pull down */
    delayUs(SPI_ACC_CS_DELAY);                       
}

void vSpiEndTransmission(){
    /* Wait until TX buffer empty, this ensures no communication is ongoing */
    while(!(SPI->SR & SPI_SR_TXE));
    
    /* Wait until SPI is not busy */
    while(SPI->SR & SPI_SR_BSY);                    
    
    /* Let the MEMS catch the SCK stable high value */
    delayUs(SPI_ACC_CS_DELAY);
    
    /* Pull up slave select, which disables the accelerometer SPI */
    cs::high();
    
    /* Let the MEMS catch the CS pull up */
    delayUs(SPI_ACC_CS_DELAY);
}
