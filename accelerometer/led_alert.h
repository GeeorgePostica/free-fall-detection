/* 
 * File:   led_alert.h
 * Author: george
 *
 * Created on January 23, 2015, 5:47 PM
 */

#ifndef LED_ALERT_H
#define	LED_ALERT_H

#define ALERT_DELAY_LOADING     100
#define ALERT_DELAY_ERROR       500
#define ALERT_DELAY_FALLING     200
#define ALERT_DELAY_CRASH       400
#define ALERT_DELAY_GRANULARITY 20

class Alert {
public:

    /* Contains all the functions of type void() to show LEDs */
    enum Alert_ {
        Loading,
        Falling,
        Error,
        Crash,
        Running
    };
private:
    Alert();
};

/* Alert initialization */
void vAlertInit();

/* Turn off or on all the LEDs in one shot */
void vSetLeds(int yellow, int red, int blue, int green);

/* Toggle all LEDs */
void vToggleLeds();

/* General Function to show alerts */
void vAlertShow(Alert::Alert_ alert);

/* Stop the current Alert*/
void vAlertStop();

#endif	/* LED_ALERT_H */

