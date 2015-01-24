#include "led_alert.h"
#include <miosix.h>
#include <pthread.h>

using namespace miosix;

typedef Gpio<GPIOD_BASE, 14> ledRed;
typedef Gpio<GPIOD_BASE, 15> ledBlue;
typedef Gpio<GPIOD_BASE, 12> ledGreen;
typedef Gpio<GPIOD_BASE, 13> ledYellow;

/* Thread running the alerts */
pthread_t alertThread;

/* Used to keep the thread running */
int running;

void vAlertInit(){
    running = 0;
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
    if (running) {
        running = 0;
        pthread_join(alertThread, NULL);
    }
    vSetLeds(0, 0, 0, 0);
}

void *AlertLoading(void *args) {
    while (running) {
        delayMs(ALERT_DELAY_LOADING/2);
        ledYellow::high();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING);
        ledYellow::low();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING/2);
        ledRed::high();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING);
        ledRed::low();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING/2);
        ledBlue::high();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING);
        ledBlue::low();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING/2);
        ledGreen::high();
        if (!running) break;
        delayMs(ALERT_DELAY_LOADING);
        ledGreen::low();
    }
    return NULL;
}

void *AlertFalling(void *args) {
    vSetLeds(1, 0, 1, 0);
    while (running) {
        delayMs(ALERT_DELAY_FALLING);
        vToggleLeds();
    }
    return NULL;
}

void *AlertCrash(void *args) {
    vSetLeds(1, 1, 1, 1);
    while (running) {
        delayMs(ALERT_DELAY_CRASH);
        vToggleLeds();
    }
    return NULL;
}

void *AlertError(void *args) {
    vSetLeds(0, 1, 0, 0);
    while (running) {
        delayMs(ALERT_DELAY_ERROR);
        ledRed::low();
        if (!running) break;
        delayMs(ALERT_DELAY_ERROR);
        ledRed::high();
    }
    return NULL;
}

void vAlertShow(Alert::Alert_ alert) {
    vAlertStop();
    running = 1;
    switch (alert) {
        case Alert::Crash:
            pthread_create(&alertThread, NULL, AlertCrash, NULL);
            break;
        case Alert::Error:
            pthread_create(&alertThread, NULL, AlertError, NULL);
            break;
        case Alert::Loading:
            pthread_create(&alertThread, NULL, AlertLoading, NULL);
            break;
        case Alert::Falling:
            pthread_create(&alertThread, NULL, AlertFalling, NULL);
            break;
        default:
            vSetLeds(0,0,0,1);
            running = 0;
            break;
    }
}
