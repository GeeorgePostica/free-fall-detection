#include <miosix.h>
#include "spi_acc.h"

using namespace miosix;

typedef Gpio<GPIOA_BASE,5> sck;
typedef Gpio<GPIOA_BASE,6> miso;
typedef Gpio<GPIOA_BASE,7> mosi;
typedef Gpio<GPIOE_BASE,3> cs;

void vSpiInit(){
    sck::mode(Mode::ALTERNATE);
    sck::alternateFunction(0);
    miso::mode(Mode::ALTERNATE);
    miso::alternateFunction(0);
    mosi::mode(Mode::ALTERNATE);
    mosi::alternateFunction(0);
    cs::mode(Mode::OUTPUT);
}

void vSpiWriteByte(char addr, char data){
    vSpiWrite(addr, *data, 1);
}

void vSpiWriteShort(char addr, unsigned short data){
    char bytes[] = { (data >> 8) , data };
    vSpiWrite(addr, bytes, 2);
}

void vSpiWriteInt(char addr, unsigned int data){
    char bytes[] = { (data >> 24) ,
                     (data >> 16) ,
                     (data >> 8)  ,
                     data           };
    vSpiWrite(addr, bytes, 4);
}

void vSpiWrite(char addr, char *data, int len){
    if(len < 1) return;
    
    sck::high();
    cs::low();
    
    char byte1 = (addr & ~SPI_ACC_RW) | SPI_ACC_MS;
    char bit = 0x80;
    
    //Sending the first byte
    for(bit; bit; bit >>= 1){
        usleep(SPI_ACC_SLEEP_us);
        sck::low();
        (byte1 & bit) ? mosi::high() : mosi::low();
        usleep(SPI_ACC_SLEEP_us);
        sck::high();
    }
    
    //Sending the (multiple) data bytes in SPI order
    char clen = (char) len;
    while(clen-- > 0){
        for(bit = 0x80; bit; bit >>= 1){
            usleep(SPI_ACC_SLEEP_us);
            sck::low();
            (*(data + clen) & bit) ? mosi::high() : mosi::low();
            usleep(SPI_ACC_SLEEP_us);
            sck::high();
        }
    }
    
    usleep(SPI_ACC_SLEEP_us);
    cs::high();
}
