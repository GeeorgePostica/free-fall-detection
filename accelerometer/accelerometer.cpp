/* 
 * File:   accelerometer.cpp
 * Author: george
 * 
 * Created on February 9, 2015, 12:48 AM
 */
#include <stdio.h>
#include "accelerometer.h"
#include "acc_controls.h"

void Accelerometer::init() {
    CTRL_REG3 = 0;
    CTRL_REG4 = 0;
    CTRL_REG5 = 0;
    CTRL_REG6 = 0;
    FIFO_CTRL = 0;
    fscale = 0;
    offset = 0;
    if (!initialized) {
        
        initialized = true;
        
        Spi::init();

        char who = Spi::read(ACC_ADDR_WHO_AM_I);
        if (who == ACC_WHO_AM_I) {
            DEBUG_LOG("\nInitializing LIS3DSH ...\n");

            /* Soft reset the accelerometer */
            reset();

            /* Print the configuration */
            DEBUG_ACC("Configuring the accelerometer: \n");
            DEBUG_ACC("-> Data ready disabled, interrupts disabled\n");
            DEBUG_ACC("-> 800Hz ODR, x/y/z enabled\n");
            DEBUG_ACC("-> 800Hz AA, no self-test, 4-wire SPI\n");
            DEBUG_ACC("-> FIFO disabled, Auto-increment enabled\n");
            DEBUG_ACC("-> FIFO turned off\n");

            /* Setting the default values in ACC struct */
            CTRL_REG3 = 0x00; // Data ready disabled, interrupts disabled
            CTRL_REG4 = 0x87; // 800Hz ODR, x/y/z enabled
            CTRL_REG5 = 0x00; // 800Hz AA, no self-test, 4-wire SPI
            CTRL_REG6 = 0x10; // FIFO disabled, Auto-increment enabled
            FIFO_CTRL = 0x00; // FIFO turned off

            /* Writing the registers of accelerometer */
            Spi::write(ACC_ADDR_CTRL_REG3, CTRL_REG3);
            Spi::write(ACC_ADDR_CTRL_REG4, CTRL_REG4);
            Spi::write(ACC_ADDR_CTRL_REG5, CTRL_REG5);
            Spi::write(ACC_ADDR_CTRL_REG6, CTRL_REG6);
            Spi::write(ACC_ADDR_FIFO_CTRL, FIFO_CTRL);

            /* Set the full scale */
            setScale(FScale::_2G);


            /* Set offsets */
            //setOffset(Axis::X, ACC_OFFSET_X);
            //setOffset(Axis::Y, ACC_OFFSET_Y);
            setOffset(Axis::Z, ACC_OFFSET_Z);

            DEBUG_LOG(" ... initialization done\n\n");
        } else if (who != 00) {
            DEBUG_LOG("\nCould not identify LIS3DSH !!\n");
        } else {
            DEBUG_LOG("\nCould not connect to LIS3DSH !!\n");
        }
        
    } else {
        DEBUG_LOG("\n Accelerometer already initialized!! \n");
    }
}

void Accelerometer::disconnect() {
    DEBUG_LOG("\n\nDisconnecting accelerometer...");
    Spi::write(ACC_ADDR_CTRL_REG6, CTRL_REG6 | (1 << 7));
    Spi::stop();
    initialized = false;
    DEBUG_LOG("  done\n");
}


void Accelerometer::reset() {
    Spi::write(ACC_ADDR_CTRL_REG3, CTRL_REG3 | 1);
}

void Accelerometer::reboot() {
    DEBUG_LOG("\n\nRebooting accelerometer...\n");
    Spi::write(ACC_ADDR_CTRL_REG6, CTRL_REG6 | (1 << 7));
    Spi::stop();
    initialized = false;
    init();
}

float Accelerometer::getX() {
    return Spi::readShort(ACC_ADDR_OUT_X_L) * fscale;
}

float Accelerometer::getY() {
    return Spi::readShort(ACC_ADDR_OUT_Y_L) * fscale;
}

float Accelerometer::getZ() {
    return Spi::readShort(ACC_ADDR_OUT_Z_L) * fscale;
}

float *Accelerometer::getXYZ(float xyz[], int samplesToAvg) {
    if (samplesToAvg < 1) {
        return NULL;
    }
    short values[3];
    int i = 0;
    while (i < samplesToAvg) {
        if (iAccIsDataReady()) {
            Spi::readShort(ACC_ADDR_OUT_X_L, values, 3);
            xyz[0] += (float) values[0];
            xyz[1] += (float) values[1];
            xyz[2] += (float) values[2];
            i++;
        }
    }
    xyz[0] *= fscale / (float) samplesToAvg;
    xyz[1] *= fscale / (float) samplesToAvg;
    xyz[2] *= fscale / (float) samplesToAvg;

    return xyz;
}

void Accelerometer::enableAxis(Axis::Axis_ axis, bool enable) {
    if (enable) {
        CTRL_REG4 |= (axis & ACC_AXIS_MASK);
    } else {
        CTRL_REG4 &= ~(axis & ACC_AXIS_MASK);
    }
    Spi::write(ACC_ADDR_CTRL_REG4, CTRL_REG4);
}

void Accelerometer::setOffset(Axis::Axis_ axis, char Offset) {
    char ax = axis == Axis::X ? ACC_ADDR_OFF_X
            : (axis == Axis::Y ? ACC_ADDR_OFF_Y : ACC_ADDR_OFF_Z);
    Spi::write(ax, Offset / offset);
}

void Accelerometer::setRate(ODR::ODR_ odr) {
    CTRL_REG4 = (CTRL_REG4 & ~ACC_ODR_MASK)
            | (odr & ACC_ODR_MASK);
    Spi::write(ACC_ADDR_CTRL_REG4, CTRL_REG4);
}

void Accelerometer::setScale(FScale::Scale scale) {
    CTRL_REG5 = (CTRL_REG5 & ~ACC_FSCALE_MASK)
            | (scale & ACC_FSCALE_MASK);
    switch (scale) {
        case ACC_CTRL_REG5_FSCALE_4G:
            fscale = ACC_SCALE_4G;
            offset = ACC_OFFSET_4G;
            DEBUG_ACC("-> FScale = 4G\n");
            break;
        case ACC_CTRL_REG5_FSCALE_6G:
            fscale = ACC_SCALE_6G;
            offset = ACC_OFFSET_6G;
            DEBUG_ACC("-> FScale = 6G\n");
            break;
        case ACC_CTRL_REG5_FSCALE_8G:
            fscale = ACC_SCALE_8G;
            offset = ACC_OFFSET_8G;
            DEBUG_ACC("-> FScale = 8G\n");
            break;
        case ACC_CTRL_REG5_FSCALE_16G:
            fscale = ACC_SCALE_16G;
            offset = ACC_OFFSET_16G;
            DEBUG_ACC("-> FScale = 16G\n");
            break;
        default:
            fscale = ACC_SCALE_2G;
            offset = ACC_OFFSET_2G;
            DEBUG_ACC("-> FScale = 2G\n");
            break;
    }
    Spi::write(ACC_ADDR_CTRL_REG5, CTRL_REG5);
}

char Accelerometer::getINFO(char infoReg) {
    return Spi::read(infoReg);
}

bool Accelerometer::isDataReady() {
    return (bool) (Spi::read(ACC_ADDR_STATUS) & (1 << 3));
}

bool Accelerometer::isDataReady(Axis::Axis_ axis) {
    return (bool) (Spi::read(ACC_ADDR_STATUS) & (axis & ACC_AXIS_MASK));
}
