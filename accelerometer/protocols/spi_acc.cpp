#include <miosix.h>
#include <stdio.h>
#include "spi_acc.h"
#include "../lib/stm32f4xx_spi.h"
#include "../lib/stm32f4xx_rcc.h"
#include "../lib/stm32f4xx_gpio.h"

using namespace miosix;


typedef Gpio<GPIOA_BASE,10> sck;
typedef Gpio<GPIOA_BASE,11> miso;
typedef Gpio<GPIOA_BASE,12> mosi;
typedef Gpio<GPIOE_BASE,14> cs;

typedef Gpio<GPIOD_BASE,13> ledSCK;   //JUST FOR TEST

/***************FUNCTION PROTOTYPES************/
int iSpiWrite(char addr, char* data, int len);
int iSpiRead(char addr, char* data, int len);
void vSpiStartTransmission();
void vSpiEndTransmission();
void vSpiSendData(char data);
char cSpiReceiveData();
/**********************************************/
void SPI_send(uint8_t address, uint8_t data)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
  
  delayMs(1);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, address);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, data);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
 
  delayMs(1);
 GPIO_SetBits(GPIOE, GPIO_Pin_3);
  
}

uint8_t SPI_read(uint8_t address)
{
  GPIO_ResetBits(GPIOE, GPIO_Pin_3);
 address = 0x80 | address;                         // 0b10 - reading and clearing status
  delayMs(1);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, address);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  SPI_I2S_ReceiveData(SPI1);
 
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET); 
  SPI_I2S_SendData(SPI1, 0x00);
  while(SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
 
  char ret = SPI_I2S_ReceiveData(SPI1);
  delayMs(1);
 GPIO_SetBits(GPIOE, GPIO_Pin_3);
  return ret;
}

void vSpiInit(){
    
  SPI_InitTypeDef SPI_InitTypeDefStruct;
  GPIO_InitTypeDef GPIO_InitTypeDefStruct;
 
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE , ENABLE);
 
  SPI_InitTypeDefStruct.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
  SPI_InitTypeDefStruct.SPI_Mode              = SPI_Mode_Master;
  SPI_InitTypeDefStruct.SPI_DataSize          = SPI_DataSize_8b;
  SPI_InitTypeDefStruct.SPI_CPOL              = SPI_CPOL_High;
  SPI_InitTypeDefStruct.SPI_CPHA              = SPI_CPHA_2Edge;
  SPI_InitTypeDefStruct.SPI_NSS               = SPI_NSS_Soft | SPI_NSSInternalSoft_Set;
  SPI_InitTypeDefStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_64;
  SPI_InitTypeDefStruct.SPI_FirstBit          = SPI_FirstBit_MSB;
  SPI_Init(SPI1, &SPI_InitTypeDefStruct);

  GPIO_InitTypeDefStruct.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_7 | GPIO_Pin_6;
  GPIO_InitTypeDefStruct.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitTypeDefStruct.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitTypeDefStruct);

  GPIO_InitTypeDefStruct.GPIO_Pin   = GPIO_Pin_3;
  GPIO_InitTypeDefStruct.GPIO_Mode  = GPIO_Mode_OUT;
  GPIO_InitTypeDefStruct.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitTypeDefStruct.GPIO_PuPd  = GPIO_PuPd_UP;
  GPIO_InitTypeDefStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_Init(GPIOE, &GPIO_InitTypeDefStruct);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

  GPIO_SetBits(GPIOE, GPIO_Pin_3);
  SPI_Cmd(SPI1, ENABLE);
  
  SPI_send(0x23, 0xc9);                         // resetting the accelerometer internal circuit
  SPI_send(0x20, 0x67);                         // 100Hz data update rate, block data update disable, x/y/z enabled 
  SPI_send(0x24, 0x20);                         // Anti aliasing filter bandwidth 800Hz, 16G (very sensitive), no self-test, 4-wire interface
  SPI_send(0x10, 0x00);                         // Output(X) = Measurement(X) - OFFSET(X) * 32;
  SPI_send(0x11, 0x00);                         // Output(Y) = Measurement(Y) - OFFSET(Y) * 32;
  SPI_send(0x12, 0x00);                         // Output(Z) = Measurement(Z) - OFFSET(Z) * 32;
  
    printf("\n SPI init status:\n SPI->CR1: 0x%08x", (unsigned int)(SPI->CR1));
    printf("\n GPIOA status: 0x%08x", (unsigned int)GPIOA->MODER);
    printf("\n GPIOE status: 0x%08x", (unsigned int)GPIOE->MODER);
    printf("\n GPIOA ALTERNATE status: 0x%08x\n\n", (unsigned int)GPIOA->AFR);
}

void vSpiWriteByte(char addr, char data){
    char byte[] = {data};
    printf("\nWriting to reg:0x%02x byte:0x%02x", addr, byte[0]);
    SPI_send(addr, data);
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
    
    printf("\n  status: CS=%01x  SPIE=%01x  SCK=%01x", 
                        cs::value(), (SPI->CR1 & SPI_CR1_SPE) != 0, sck::value());
    printf("\n  sent byte: \t0x%02x\n", addr);
    
    //Sending the first byte
    vSpiSendData(addr);
    printf("  received: \t");
   
    //Sending the (multiple) data bytes in SPI order
    int i = 0;
    while(i < len){
        vSpiSendData(data[i++]);    /* Send a byte of data */
        cSpiReceiveData();          /* Dummy read */
    }
    
    while((SPI->SR & SPI_SR_RXNE)) cSpiReceiveData();   //Clear the RXNE line               
    vSpiEndTransmission();
    printf("\n  status: CS=%01x  SPIE=%01x  SCK=%01x", 
                        cs::value(), (SPI->CR1 & SPI_CR1_SPE) != 0, sck::value());
    printf("\n");
    return 1;
}

char cSpiReadByte(char addr){
    char byte[] = { 0 };
    printf("\nReading from reg: 0x%02x ...", addr);
    byte[0] = SPI_read(addr);
    printf("\n  received: 0x%02x\n", byte[0]);
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
    printf("\n  status: CS=%01x  SPIE=%01x  SCK=%01x", 
                        cs::value(), (SPI->CR1 & SPI_CR1_SPE) != 0, sck::value());
    
    printf("\n  sent byte: \t0x%02x\n", byte1);
    
    //Send the address
    vSpiSendData(byte1);
    printf("  received: \t");
   
    vSpiSendData(byte1);        //Dummy write
    byte1 = cSpiReceiveData();
    
    //Receiving the (multiple) data bytes in SPI order
    while(i < len-1){
        vSpiSendData(0x00);
        data[i++] = cSpiReceiveData();  /* NOTE: that data is received in 
                                              * SPI mode that is: from the LS byte
                                              * to the MS byte */
    }
    data[i] = cSpiReceiveData();  
    while(SPI->SR & SPI_SR_RXNE) cSpiReceiveData(); //Clear the RXNE line
    vSpiEndTransmission();
    printf("\n  status: CS=%01x  SPIE=%01x  SCK=%01x", 
                        cs::value(), (SPI->CR1 & SPI_CR1_SPE) != 0, sck::value());
    printf("\n");
    return 1;
}

void vSpiSendData(char data){
    while((SPI->SR & SPI_SR_TXE) == 0);
    SPI->DR = data;
}

char cSpiReceiveData(){
    uint8_t rx;
    while((SPI->SR & SPI_SR_RXNE) == 0);
    rx = SPI->DR;
    printf("0x%02x\t", rx);
    return rx;
}

void vSpiStartTransmission(){
    ledSCK::high();
    cs::low();                                      /* Enable slave select */
    Thread::sleep(1);
    while(cs::value());
    //SPI->CR1 |= SPI_CR1_SPE;                        /* Enable the SPI */
    //while(!(SPI->CR1 & SPI_CR1_SPE));
    
}

void vSpiEndTransmission(){
    while(!(SPI->SR & SPI_SR_TXE));                 /* Wait until TX buffer empty */
    while(SPI->SR & SPI_SR_BSY);                    /* Wait until SPI not busy */
    //SPI->CR1 &= ~SPI_CR1_SPE;                       /* Disable the SPI */
    //while((SPI->CR1 & SPI_CR1_SPE));
    Thread::sleep(1);
    cs::high();                                     /* Disable slave select */
    while(!cs::value());
    ledSCK::low();
}
