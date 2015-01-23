/* 
 * File:   Notifier.cpp
 * Author: george
 * 
 * Created on January 23, 2015, 12:33 PM
 */

#include "Notifier.h"

#define N_DELAY_INIT        500
#define N_DELAY_LOAD        400
#define N_DELAY_ERROR       500
#define N_DELAY_WARNING     400
#define N_DELAY_CRITICAL    300


Notifier::Notifier() {
    
}

Notifier* Notifier::Instance() {
    if(!instance){
        instance = new Notifier();
    }
    return instance;
}

void Notifier::showCritical() {
    toggleLeds(1, 1, 1, 1);
    delayMs(N_DELAY_CRITICAL);
    toggleLeds(0, 0, 0, 0);
    delayMs(N_DELAY_CRITICAL);
}

void Notifier::showDirection(float xyz[], float threshold) {
    toggleLeds(0,0,0,0);
    /* Positive X direction */
    xyz[0] > threshold ? ledGreen::high() : ledGreen::low();
    /* Negative X direction */
    xyz[0] < -threshold ? ledRed::high() : ledRed::low();
    /* Positive Y direction */
    xyz[1] > threshold ? ledBlue::high() : ledBlue::low();
    /* Negative Y direction */
    xyz[1] < -threshold ? ledYellow::high() : ledYellow::low();
}

void Notifier::showError() {
    toggleLeds(0, 1, 0, 0)
    delayMs(N_DELAY_ERROR);
    ledRed::low();
    delayMs(N_DELAY_ERROR);
}

void Notifier::showInitialization() {
    delayMs(N_DELAY_INIT);
    toggleLeds(1, 0, 1, 0);
    delayMs(N_DELAY_INIT);
    toggleLeds(0, 1, 0, 1);
}

void Notifier::showLoading() {
    delayMs(N_DELAY_LOAD);
    ledYellow::high();
    delayMs(N_DELAY_LOAD);
    ledBlue::low();
    delayMs(N_DELAY_LOAD);
    ledRed::high();
    delayMs(N_DELAY_LOAD);
    ledGreen::low();
    delayMs(N_DELAY_LOAD);
    ledBlue::high();
    delayMs(N_DELAY_LOAD);
    ledYellow::low();
    delayMs(N_DELAY_LOAD);
    ledGreen::high();
    delayMs(N_DELAY_LOAD);
    ledRed::low();
}

void Notifier::showWarning() {
    toggleLeds(0, 1, 0, 1);
    delayMs(N_DELAY_WARNING);
    toggleLeds(1, 1, 1, 1);
    delayMs(N_DELAY_WARNING);
    toggleLeds(1, 0, 1, 0);
    delayMs(N_DELAY_WARNING);
    toggleLeds(1, 1, 1, 1);
    delayMs(N_DELAY_WARNING);
}

void Notifier::toggleLeds(int yellow, int red, int blue, int green) {
    red     ?   ledRed::high()      :   ledRed::low();
    blue    ?   ledBlue::high()     :   ledBlue::low();
    green   ?   ledGreen::high()    :   ledGreen::low();
    yellow  ?   ledYellow::high()   :   ledYellow::low();
}
