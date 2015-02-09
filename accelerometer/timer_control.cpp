#include "timer_control.h"

#define RESET   0x0000
#define MAX_ARR 0xffffffff

void (*cbFunctionTIM2)();
void (*cbFunctionTIM3)();
void (*cbFunctionTIM4)();

void TIM2_IRQHandler(){
    
}

void TIM3_IRQHandler(){
    
}

void TIM4_IRQHandler(){
    
}

Timer* Timer::Use(HWTimer timer) {
    if(!timers[timer]){
        timers[timer] = new Timer(timer);
    }
    return timers[timer];
}

Timer* Timer::Use(TIM_TypeDef hwTimer){
    if(hwTimer == TIM3){
        return Use(HWTimer::timer3);
    }
    else if(hwTimer == TIM4){
        return Use(HWTimer::timer4);
    }
    return Use(HWTimer::timer2);
}

Timer::Timer(int timerIndex) {
    //Initialize timer
    index = timerIndex;
    TIMer = TIMS[timerIndex];
    
}

Timer::~Timer() {
    
}

void Timer::start(Resolution res) {
    TIMer->DIER = RESET;       //Disable interrupt
    TIMer->ARR = MAX_ARR;      //Set the max value
    //TIMer->CR1 |= TIM_CR1_ARPE; //Enable shadow register
    TIMer->CR1 |= TIM_CR1_URS  //Enable update request source
                | TIM_CR1_CEN; //Enable the timer
}

void Timer::start(long tickTimeUs, void(*callbackFunction)()) {
    
    
    setCallbackFunction(callbackFunction);
}

void Timer::setCallbackFunction(void(*callbackFunction)()) {
    switch(index){
        case 0:
            cbFunctionTIM2 = callbackFunction;
            break;
        case 1:
            cbFunctionTIM3 = callbackFunction;
            break;
        default:
            cbFunctionTIM4 = callbackFunction;
            break;
    }
}

void Timer::stop() {

}

unsigned int Timer::elapsedTimeUs() {
    return (unsigned int) TIMer->CNT;
}

void Timer::init() {
    if(timer5 == NULL){
       timer5 = new Timer(); 
    }
}

void Timer::halt() {

}

long int Timer::time() {
    
}

