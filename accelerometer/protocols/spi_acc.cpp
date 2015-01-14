#include <miosix.h>
#include "spi_acc.h"

using namespace miosix;

typedef Gpio<GPIOA_BASE,5> sck;
typedef Gpio<GPIOA_BASE,6> miso;
typedef Gpio<GPIOA_BASE,7> mosi;
typedef Gpio<GPIOE_BASE,3> cs;

int iSpiNextSCKValue;

void vSpiInit(){
    sck::mode(Mode::ALTERNATE);
    sck::alternateFunction(0);
    miso::mode(Mode::ALTERNATE);
    miso::alternateFunction(0);
    mosi::mode(Mode::ALTERNATE);
    mosi::alternateFunction(0);
    cs::mode(Mode::OUTPUT);
    cs::high();
    sck::high();
    iSpiNextSCKValue = 1;
}

void vSpiWriteByte(char addr, char data){
    iSpiWrite(addr, *data, 1);
}

void vSpiWriteShort(char addr, unsigned short data){
    char bytes[] = { (data >> 8) , data };
    iSpiWrite(addr, bytes, 2);
}

void vSpiWriteInt(char addr, unsigned int data){
    char bytes[] = { (data >> 24) ,
                     (data >> 16) ,
                     (data >> 8)  ,
                     data           };
    iSpiWrite(addr, bytes, 4);
}

int iSpiWrite(char addr, char *data, int len){
    if(len < 1) return 0;
    
    char byte1 = (addr & ~SPI_ACC_RW) | SPI_ACC_MS;
    
    vSpiStartTransmission();
    
    //Sending the first byte
    vSpiSendData(byte1);
    
    //Sending the (multiple) data bytes in SPI order
    char clen = (char) len;
    while(clen-- > 0){
        vSpiSendData(*(data + clen));
    }
    
    vSpiEndTransmission();
    return 1;
}

void vSpiReadByte(char addr, char &data){
    iSpiRead(addr, *data, 1);
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

void vSpiReadShort(char addr, short &data){
    char bytes[2];
    iSpiRead(addr, bytes, 2);
    data = bytes[1]*0x100 + bytes[0];
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
    
    char byte1 = (addr | SPI_ACC_RW) | SPI_ACC_MS;
    
    vSpiStartTransmission();
    
    //Send the address
    vSpiSendData(byte1);
    
    //Receiving the (multiple) data bytes in SPI order
    char clen = (char) len;
    while(clen-- > 0){
        *(data + clen) = vSpiReceiveData();  /* NOTE: that data is received in 
                                              * SPI mode that is: from the LS byte
                                              * to the MS byte */
    }
    
    vSpiEndTransmission();
    return 1;
}

void vSpiSendData(char data){
    vSpiAccSckLow();
    for(char bit = 0x80; bit; bit >>= 1){
        (data & bit) ? mosi::high() : mosi::low();
        vSpiAccSckHigh();
        vSpiAccSckLow();
    }
}

char vSpiReceiveData(){
    char data;
    vSpiAccSckHigh();
    for(char bit = 0x80; bit; bit >>= 1){
        if(miso::value()){
            data |= 1 << (bit-1);
        }
        vSpiAccSckLow();
        vSpiAccSckHigh();
    }
    return data;
}

void vSpiAccSckHigh(){
    if(!iSpiNextSCKValue){
        while(sck::value());
        vSpiDelay();
        sck::high();
        iSpiNextSCKValue = 1;
    }
}

void vSpiAccSckLow(){
    if(iSpiNextSCKValue){
        while(!sck::value());
        vSpiDelay();
        sck::low();
        iSpiNextSCKValue = 0;
    }
}

void vSpiDelay(){
    usleep(SPI_ACC_SLEEP_us);
}

void vSpiStartTransmission(){
    if(!sck::value()){
        sck::high();
        while(!sck::value());
    }
    cs::low();
}

void vSpiEndTransmission(){
    if(!sck::value()){
        sck::high();
        while(!sck::value());
    }
    vSpiDelay();
    cs::high();
}
