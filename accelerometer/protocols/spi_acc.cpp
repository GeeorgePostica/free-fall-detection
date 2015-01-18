#include <miosix.h>
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
    sck::mode(Mode::ALTERNATE);
    sck::alternateFunction(5);
    miso::mode(Mode::ALTERNATE);
    miso::alternateFunction(5);
    mosi::mode(Mode::ALTERNATE);
    mosi::alternateFunction(5);
    //cs::mode(Mode::ALTERNATE);
    //cs::alternateFunction(5);
    
    if(SPI == SPI1){
        RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;             /* Enable clock for SPI */
    }
    else if(SPI == SPI2){
        RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;             /* Enable clock for SPI */
    }
    else {
        RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;             /* Enable clock for SPI */
    }
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;            /* Enable clock on GPIOA */
    //RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN;            /* Enable clock on GPIOE */
    
    SPI->CR1 &= ~(SPI_CR1_CPHA | SPI_CR1_CPOL);    /* Set the CPHA=0 and CPOL=0 */
    SPI->CR1 |= SPI_CR1_MSTR;                      /* Set the master mode */
    SPI->CR1 |= SPI_CR1_BIDIMODE;                  /* Enable Full Duplex */
    //SPI->CR1 |= SPI_CR1_SSI;                       /* Enable auto slave select */
    //SPI->CR2 |= SPI_CR2_SSOE;                      /* Enable SS output */
    SPI->CR1 |= SPI_CR1_SSM;                        /* Manage software slave select */
    //SPI->CR1 |= SPI_CR1_DFF;                        /* LSB first */
    SPI->CR1 |= SPI_CR1_BR_0 | SPI_CR1_BR_1 | SPI_CR1_BR_2;     /* Set the baud pre multiplier to 32 */
    SPI->CR2 |= SPI_CR2_FRF;                       /* 16bit frame */
    
    SPI->CR1 |= SPI_CR1_SPE;                       /* Enable the SPI */
    
    //SPI->DR = 0;                                    /* Empty the buffer */
    
    cs::mode(Mode::OUTPUT);
    cs::high();
    
    ledSCK::mode(Mode::OUTPUT);
    ledSCK::low();
    ledRX::mode(Mode::OUTPUT);
    ledRX::low();
}

void vSpiWriteByte(char addr, char data){
    char byte[] = {data};
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
    
    char byte1 = (addr & ~SPI_ACC_RW) & ~SPI_ACC_MS;    /* Set the write bit */
    
    vSpiStartTransmission();
    
    //Sending the first byte
    vSpiSendData(byte1);
    //while(!(SPI->SR & SPI_SR_TXE));
    //SPI->DR = byte1;
    
    //while(!(SPI->SR & SPI_SR_RXNE));
    cSpiReceiveData();      /* Wait for the incoming useless data */
    //SPI->DR = 0;        /* Empty the data buffer */
    
    //Sending the (multiple) data bytes in SPI order
    while(len-- > 0){
        vSpiSendData(data[len]);    /* Send a byte of data */
        cSpiReceiveData();          /* Discard the useless data */
    }
    
    vSpiEndTransmission();
    return 1;
}

char cSpiReadByte(char addr){
    char byte[] = { 0 };
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
    
    char byte1 = (addr | SPI_ACC_RW) & ~SPI_ACC_MS;
    
    vSpiStartTransmission();
    
    //Send the address
    vSpiSendData(byte1);
    
    cSpiReceiveData();
    
    //Receiving the (multiple) data bytes in SPI order
    while(len-- > 0){
        vSpiSendData(0x00);
        data[len] = cSpiReceiveData();  /* NOTE: that data is received in 
                                              * SPI mode that is: from the LS byte
                                              * to the MS byte */
    }
    
    vSpiEndTransmission();
    return 1;
}

void vSpiSendData(char data){
    while(!(SPI->SR & SPI_SR_TXE));
    //ledRX::high();
    SPI->DR = data;
    //ledRX::low();
}

char cSpiReceiveData(){
    char data;
    //ledRX::low();
    while(!(SPI->SR & SPI_SR_RXNE));
    //ledRX::low();
    data = SPI->DR;
    if(SPI->DR) ledRX::high();
    //SPI->DR = 0;
    return data;
}

void vSpiStartTransmission(){
    cs::low();
    //SPI->CR1 |= SPI_CR1_SPE;
    ledSCK::high();
}

void vSpiEndTransmission(){
    //SPI->CR1 &= ~SPI_CR1_SPE;
    cs::high();
    ledSCK::low();
}
