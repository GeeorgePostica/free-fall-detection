/* 
 * File:   Notifier.h
 * Author: george
 *
 * Created on January 23, 2015, 12:33 PM
 */

#ifndef NOTIFIER_H
#define	NOTIFIER_H

#include <miosix.h>
#include <pthread.h>

using namespace miosix;

class Notifier {
public:
    static Notifier* Instance();
    void showInitialization();
    void showLoading();
    void showDirection(float xyz[], float threshold);
    void showCritical();
    void showWarning();
    void showError();
private:
    typedef Gpio<GPIOD_BASE, 14> ledRed;
    typedef Gpio<GPIOD_BASE, 15> ledBlue;
    typedef Gpio<GPIOD_BASE, 12> ledGreen;
    typedef Gpio<GPIOD_BASE, 13> ledYellow;
    pthread_t thread;
    static Notifier* instance;
    Notifier();
    void toggleLeds(int yellow, int red, int blue, int green);
};

#endif	/* NOTIFIER_H */

