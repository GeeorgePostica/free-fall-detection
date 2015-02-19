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

void vAlertStop() {
    vSetLeds(0, 0, 0, 0);
}

void vAlertShow(Alert::Alert_ alert) {
    switch (alert) {
        case Alert::Crash:
            vSetLeds(1, 1, 1, 1);
            break;
        case Alert::Error:
            vSetLeds(0, 1, 0, 0);
            break;
        case Alert::Loading:
            vSetLeds(1, 0, 0, 0);
            break;
        case Alert::Falling:
            vSetLeds(1, 0, 1, 0);
            break;
        default:
            vSetLeds(0, 0, 0, 1);
            break;
    }
}