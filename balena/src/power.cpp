
#include "power.h"

// int test = 1;
// CM3PowerState state;


PowerClass::PowerClass()
{
    timer.sleep_delay = NULL;
    timer.sleep_period = NULL;
    timer.sleep_mode = EM0;
    timer.wake_mode = EM0;
    timer.cm3_state = POWER_OFF_DELAY;
}

PowerClass::PowerClass(uint32_t delay, uint32_t period, CoprocessorEnergyMode cp_sleep_mode, CoprocessorEnergyMode cp_wake_mode){
    timer.sleep_delay = delay;
    timer.sleep_period = period;
    timer.sleep_mode = cp_sleep_mode;
    timer.wake_mode = cp_wake_mode;
};

void PowerClass::Init(uint32_t delay, uint32_t period, CoprocessorEnergyMode cp_sleep_mode, CoprocessorEnergyMode cp_wake_mode){
    timer.sleep_delay = delay;
    timer.sleep_period = period;
    timer.sleep_mode = cp_sleep_mode;
    timer.wake_mode = cp_wake_mode;
};

void PowerClass::PowerDown(){
    if((timer.sleep_period) != (0 | NULL)){
        switch (timer.sleep_delay > 0 ? 0 : 1){
                case (0):
                    timer.cm3_state = POWER_OFF_DELAY;
                    rtchandle.toggle = timer.cm3_state;
                    rtchandle.sleep_period = timer.sleep_period;
                    digitalWrite(14,0);
                    RTCDRV_StartTimer(id, rtcdrvTimerTypeOneshot, timer.sleep_delay, WakeEvent, &rtchandle);
                    break;
                case (1):
                    timer.cm3_state = POWER_OFF;
                    rtchandle.toggle = timer.cm3_state;
                    RTCDRV_StartTimer(id, rtcdrvTimerTypeOneshot, timer.sleep_period, WakeEvent, &rtchandle);
                    break;
                default:
                    timer.cm3_state = POWER_ON;
                    break;
            }
    }
    else {
        throw "Invalid sleep period";
    }
}

static void WakeEvent(RTCDRV_TimerID_t id, void * user)
{
    RTCHandle * power_timer = (RTCHandle*) user;
    if(power_timer->toggle == POWER_OFF_DELAY){
        digitalWrite(SLEEP_PIN, 0);
        power_timer->toggle = POWER_OFF;
        RTCDRV_StartTimer(id, rtcdrvTimerTypeOneshot, power_timer->sleep_period, WakeEvent, &power_timer);
    }
    else {
        digitalWrite(SLEEP_PIN, 1);
    }
}
