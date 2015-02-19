#include "led_alert.h"
#include <miosix.h>

using namespace miosix;

typedef Gpio<GPIOD_BASE, 14> ledRed;
typedef Gpio<GPIOD_BASE, 15> ledBlue;
typedef Gpio<GPIOD_BASE, 12> ledGreen;
typedef Gpio<GPIOD_BASE, 13> ledYellow;

void vAlertInit(){
    ledRed::mode(Mode::OUTPUT);
    ledBlue::mode(Mode::OUTPUT);
    ledGreen::mode(Mode::OUTPUT);
    ledYellow::mode(Mode::OUTPUT);
}

void vSetLeds(int yellow, int red, int blue, int green) {
    red ? ledRed::high() : ledRed::low();
    blue ? ledBlue::high() : ledBlue::low();
    green ? ledGreen::high() : ledGreen::low();
    yellow ? ledYellow::high() : ledYellow::low();
}

void vToggleLeds() {
    !ledRed::value() ? ledRed::high() : ledRed::low();
    !ledBlue::value() ? ledBlue::high() : ledBlue::low();
    !ledGreen::value() ? ledGreen::high() : ledGreen::low();
    !ledYellow::value() ? ledYellow::high() : ledYellow::low();
}

void vAlertStop() {
    vSetLeds(0, 0, 0, 0);
}

void AlertLoading() {
    vSetLeds(1, 0, 0, 0);
}

void AlertFalling() {
    vSetLeds(1, 0, 1, 0);
}

void AlertCrash() {
    vSetLeds(1, 1, 1, 1);
}

void AlertError() {
    vSetLeds(0, 1, 0, 0);
}

void nothing(){
    vSetLeds(0, 0, 0, 1);
}

void vAlertShow(Alert::Alert_ alert) {
    switch (alert) {
        case Alert::Crash:
            AlertCrash();
            break;
        case Alert::Error:
            AlertError;
            break;
        case Alert::Loading:
            AlertLoading;
            break;
        case Alert::Falling:
            AlertFalling;
            break;
        default:
            nothing;
            break;
    }
}