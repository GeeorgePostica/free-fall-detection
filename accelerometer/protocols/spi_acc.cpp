#include <miosix.h>
#include <stdio.h>
#include "spi_acc.h"

using namespace miosix;

typedef Gpio<GPIOA_BASE,5> sck;
typedef Gpio<GPIOA_BASE,6> miso;
typedef Gpio<GPIOA_BASE,7> mosi;
typedef Gpio<GPIOE_BASE,3> cs;

typedef Gpio<GPIOD_BASE,13> ledSCK;   //JUST FOR TEST
typedef Gpio<GPIOD_BASE,12> ledRX;   //JUST FOR TEST

int iSpiNextSCKValue;

/***************FUNCTION PROTOTYPES************/
int iSpiWrite(char addr, char* data, int len);
int iSpiRead(char addr, char* data, int len);
void vSpiStartTransmission();
void vSpiEndTransmission();
void vSpiSendData(char data);
char cSpiReceiveData();
/**********************************************/

void vSpiInit(){
    
    if(SPI == SPI1){
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;             /* Enable clock for SPI */
    }
    else if(SPI == SPI2){
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;             /* Enable clock for SPI */
    }
    else {
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;             /* Enable clock for SPI */
    }
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;                /* Enable clock on GPIOA */
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;                /* Enable clock on GPIOE */
    
    SPI->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);         /* Set the CPHA=0 and CPOL=0 */
    SPI->CR1 |= SPI_CR1_MSTR;                           /* Set the master mode */
    SPI->CR1 &= ~SPI_CR1_BIDIMODE;                    /* Enable Full Duplex */
    SPI->CR1 &= ~SPI_CR1_RXONLY;                        /*Not receive only */
    SPI->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;              /* Manage software slave select */
    SPI->CR1 &= ~SPI_CR1_LSBFIRST;                       /* MSB first */
    SPI->CR1 &= ~SPI_CR1_DFF;                          /* 8bit format */
    SPI->CR1 |= SPI_CR1_BR_2 | SPI_CR1_BR_0;                            /* Set the baud pre multiplier to 64 */
    SPI->CR2 &= ~SPI_CR2_FRF;                            /* Motorola mode */
    
    sck::mode(Mode::ALTERNATE);
    sck::alternateFunction(5);
    miso::mode(Mode::ALTERNATE);
    miso::alternateFunction(5);
    mosi::mode(Mode::ALTERNATE);
    mosi::alternateFunction(5);
    
    cs::mode(Mode::OUTPUT);
    cs::high();
    
    ledSCK::mode(Mode::OUTPUT);
    ledSCK::low();
    ledRX::mode(Mode::OUTPUT);
    ledRX::low();
}

void vSpiWriteByte(char addr, char data){
    char byte[] = {data};
    printf("\n----------------\nWriting to reg:0x%02x byte:0x%02x\n", addr, byte[0]);
    iSpiWrite(addr, byte, 1);
}

void vSpiWriteShort(char addr, unsigned short data){
    char bytes[] = { (char)(data >> 8) , (char)data };
    iSpiWrite(addr, bytes, 2);
}

void vSpiWriteInt(char addr, unsigned int data){
    char bytes[] = { (char)(data >> 24) ,
                     (char)(data >> 16) ,
                     (char)(data >> 8)  ,
                     (char)data           };
    iSpiWrite(addr, bytes, 4);
}

int iSpiWrite(char addr, char* data, int len){
    if(len < 1) return 0;
    
    vSpiStartTransmission();
    
    printf("sent byte: \t0x%02x\n", addr);
    
    //Sending the first byte
    SPI->DR = addr;
    
    //Sending the (multiple) data bytes in SPI order
    int i = 0;
    while(i < len){
        vSpiSendData(data[i++]);    /* Send a byte of data */
    }
    
    vSpiEndTransmission();
    return 1;
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
    if(len < 1) return 0;
    
    char byte1 = (addr | SPI_ACC_RW);
    int i = 0;
    
    vSpiStartTransmission();
    
    printf("\nsent byte: \t0x%02x\n", byte1);
    
    //Send the address
    SPI->DR = byte1;
    
    vSpiSendData(0x00);
    byte1 = cSpiReceiveData();
    printf("received: \t0x%02x\t", byte1);
    
    //Receiving the (multiple) data bytes in SPI order
    while(i < len-1){
        vSpiSendData(0x00);
        data[i++] = cSpiReceiveData();  /* NOTE: that data is received in 
                                              * SPI mode that is: from the LS byte
                                              * to the MS byte */
        printf("0x%02x\t", data[i]);
    }
    data[i] = cSpiReceiveData();  
    printf("0x%02x\n", data[i]);
    
    vSpiEndTransmission();
    return 1;
}

void vSpiSendData(char data){
    while(!(SPI->SR & SPI_SR_TXE));
    SPI->DR = data;
}

char cSpiReceiveData(){
    while(!(SPI->SR & SPI_SR_RXNE));
    return SPI->DR;
}

void vSpiStartTransmission(){
    ledSCK::high();
    SPI->CR1 |= SPI_CR1_SPE;                        /* Enable the SPI */   
    cs::low();                                     /* Enable slave select */
}

void vSpiEndTransmission(){
    cs::high();                                      /* Disable slave select */
    //while(!(SPI->SR & SPI_SR_RXNE));
    while(!(SPI->SR & SPI_SR_TXE));                 /* Wait until TX buffer empty */
    while(SPI->SR & SPI_SR_BSY);                    /* Wait until SPI not busy */
    SPI->CR1 &= ~SPI_CR1_SPE;                       /* Disable the SPI */
    ledSCK::low();
}
