#include "led_alert.h"
#include <miosix.h>
#include <pthread.h>

using namespace miosix;

/*********** FUNCTION PROTOTYPES *************/
void *threadRun(void* args);
void switchAlert(void(*nextAlert)());
int cannotContinue(int checkPeriod);
/*********************************************/

typedef Gpio<GPIOD_BASE, 14> ledRed;
typedef Gpio<GPIOD_BASE, 15> ledBlue;
typedef Gpio<GPIOD_BASE, 12> ledGreen;
typedef Gpio<GPIOD_BASE, 13> ledYellow;

/* Thread running the alerts */
pthread_t alertThread;

/* Mutex for synch */
pthread_mutex_t alertMutex;

/* Needed to change execution function */
int leds_running;

/* Used to keep the thread running */
int alive;

/* Used to avoid multiple initializations */
int leds_started = 0;

/* Pointer to function executing by the thread */
void (*lightUp)();

/* Pointer to function to be executed by the thread */
void (*nextLights)();

void vAlertInit() {
    if (!leds_started) {
        leds_started = 1;
        leds_running = 0;
        ledRed::mode(Mode::OUTPUT);
        ledBlue::mode(Mode::OUTPUT);
        ledGreen::mode(Mode::OUTPUT);
        ledYellow::mode(Mode::OUTPUT);
        
        pthread_mutex_init(&alertMutex, NULL);
        
        alive = 1;
        pthread_create(&alertThread, NULL, threadRun, NULL);
    }
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
    leds_started = 0;
    pthread_mutex_lock(&alertMutex);
    alive = 0;
    leds_running = 0;
    pthread_mutex_unlock(&alertMutex);
    pthread_join(alertThread, NULL);
    pthread_mutex_destroy(&alertMutex);
    vSetLeds(0, 0, 0, 0);
}

void AlertLoading() {
    while (leds_running) {
        if (cannotContinue(ALERT_DELAY_LOADING / 2)) break;
        ledYellow::high();
        if (cannotContinue(ALERT_DELAY_LOADING)) break;
        ledYellow::low();
        if (cannotContinue(ALERT_DELAY_LOADING / 2)) break;
        ledRed::high();
        if (cannotContinue(ALERT_DELAY_LOADING)) break;
        ledRed::low();
        if (cannotContinue(ALERT_DELAY_LOADING / 2)) break;
        ledBlue::high();
        if (cannotContinue(ALERT_DELAY_LOADING)) break;
        ledBlue::low();
        if (cannotContinue(ALERT_DELAY_LOADING / 2)) break;
        ledGreen::high();
        if (cannotContinue(ALERT_DELAY_LOADING)) break;
        ledGreen::low();
    }
}

void AlertFalling() {
    vSetLeds(1, 0, 1, 0);
    while (!cannotContinue(ALERT_DELAY_FALLING)) {
        vToggleLeds();
    }
}

void AlertCrash() {
    vSetLeds(1, 1, 1, 1);
    while (!cannotContinue(ALERT_DELAY_CRASH)) {
        vToggleLeds();
    }
}

void AlertError() {
    vSetLeds(0, 1, 0, 0);
    while (!cannotContinue(ALERT_DELAY_ERROR)) {
        ledRed::low();
        if (cannotContinue(ALERT_DELAY_ERROR)) break;
        ledRed::high();
    }
}

void AlertRunning() {
    vSetLeds(0, 0, 0, 1);
    while (leds_running) {
        Thread::sleep(10);
    }
}

void vAlertShow(Alert::Alert_ alert) {
    switch (alert) {
        case Alert::Crash:
            switchAlert(AlertCrash);
            break;
        case Alert::Error:
            switchAlert(AlertError);
            break;
        case Alert::Loading:
            switchAlert(AlertLoading);
            break;
        case Alert::Falling:
            switchAlert(AlertFalling);
            break;
        default:
            switchAlert(AlertRunning);
            break;
    }
}

void switchAlert(void (*nextAlert)()) {
    pthread_mutex_lock(&alertMutex);
    nextLights = nextAlert;
    leds_running = 0;
    pthread_mutex_unlock(&alertMutex);
}

void *threadRun(void *args) {
    lightUp = AlertRunning;
    int loop;
    pthread_mutex_lock(&alertMutex);
    leds_running = 1;
    loop = alive;
    pthread_mutex_unlock(&alertMutex);
    while (loop) {
        pthread_mutex_lock(&alertMutex);
        lightUp = nextLights;
        leds_running = 1;
        pthread_mutex_unlock(&alertMutex);
        
        lightUp();
        
        pthread_mutex_lock(&alertMutex);
        loop = alive;
        pthread_mutex_unlock(&alertMutex);
    }
    return NULL;
}

#ifdef ALERT_DELAY_GRANULARITY

int cannotContinue(int checkPeriod) {
    while (checkPeriod > 0) {
        pthread_mutex_lock(&alertMutex);
        if (!leds_running) {
            pthread_mutex_unlock(&alertMutex);
            return 1;
        }
        pthread_mutex_unlock(&alertMutex);
        Thread::sleep(ALERT_DELAY_GRANULARITY);
        checkPeriod -= ALERT_DELAY_GRANULARITY;
    }
    return 0;
}
#else

int cannotContinue(int checkPeriod) {
    delayMs(checkPeriod);
    return !leds_running;
}
#endif