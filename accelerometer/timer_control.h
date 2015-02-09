/* 
 * File:   timer_control.h
 * Author: george
 *
 * Created on January 23, 2015, 9:50 PM
 */

#ifndef TIMER_CONTROL_H
#define	TIMER_CONTROL_H

#include <miosix.h>

#define SYSTEM_TIMER_CLOCK 168000000 //CHECK the TIM BUS CLOCK
#define TIMER_1us_PRESCALER SYSTEM_TIMER_CLOCK/1000000

using namespace miosix;

class Timer{
public:
    enum HWTimer{
        timer2 = 0,
        timer3 = 1,
        timer4 = 2
    };
    
    enum Resolution{
        r100us      = 100 * TIMER_1us_PRESCALER,  //Max counting value: ~6.5 seconds (16bit TIM)
        r10us       = 10 * TIMER_1us_PRESCALER,   //Max counting value: ~650 ms (16bit TIM)
        r1us        = 1 * TIMER_1us_PRESCALER     //Max counting value: ~65 ms (16bit TIM)
    };
    
    /**Automatically configures (1us resolution) the specified 
     * General Purpose HW Timer (TIM2: 32bit; TIM3, TIM4: 16bit)
     * and returns a Timer object */
    static Timer* Use(HWTimer timer);
    
    /**Automatically configures (1us resolution) the specified 
     * General Purpose HW Timer (TIM2: 32bit; TIM3, TIM4: 16bit) of 
     * <b>TIM_TypeDef</b> type and returns a Timer object
     * <br> <i>If requested timer is not from TIM2 to TIM4 then
     *  <b>TIM2</b> is returned</i> */
    static Timer* Use(TIM_TypeDef* hwTimer);
    
    /** Starts the timer in a simple counting mode
     * \param resolutionUs */
    void start(Resolution res);
    
    /** Starts the timer in interrupt mode, upon reaching tickTimeUs,
     *  an interrupt is raised and the callbackFunction is executed */
    void start(long tickTimeUs, void (*callbackFunction)());
    
    /** Stops the current timer */
    void stop();
    
    /** Returns the elapsed time (in us) since the timer started */
    unsigned int elapsedTimeUs();
    
    TIM_TypeDef* getGPTIM(){
        return TIMS[index];
    }
    
    /** Returns the elapsed time since the Timer.init() was called */
    static long int time();
    
    /** Initializes the timer TIM5 (which is 32bit) in 'upcount' mode
     * <br> It is useful when calculating execution time
     * <br> <i> A subsequent call of this method won't restart the timer</i>
     *  thus the halt() method should be called first */
    static void init();
    
    /** Stops the timer TIM5 from counting, disables and releases the HW TIM5*/
    static void halt();
    
private:
    TIM_TypeDef* TIMer;
    int index;
    void (*callFunction)();
    
    Timer(int timerIndex);
    
    /** DESTRUCTOR: Releases a GP HW Timer */
    ~Timer();
    
    void setCallbackFunction(void (*callbackFunction)());
    
    /*--------------- STATIC PART ---------------------------------*/
    static Timer timer5;
    
    static const bool intEnabled = { false, false, false, false };
    static const TIM_TypeDef* TIMS[] = { TIM2, TIM3, TIM4, TIM5 };
    static const Timer* timers[4];
    /*--------------------------------------------------------------*/
    
};

#endif	/* TIMER_CONTROL_H */

