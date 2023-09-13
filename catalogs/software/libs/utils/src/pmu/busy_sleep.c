#include "core/core.h"

extern void Timers_OneShotTimer(uint32_t microseconds_);
extern volatile uint32_t Timers_OneShotTriggered;


void Utils_BusyMicroSleep(uint64_t useconds) {
  if(Timers_OneShotTriggered == 0xFFFFFFFF) {
    // use a badly calibrated for loop (TODO make sure optimiser doesn't remove)
    for(uint32_t count = 0U; count < useconds * 180U; ++count) {
    }
  } else {
    // as interrupt and timers are setup use a delay timer 
    Timers_OneShotTimer(useconds);
    while(Timers_OneShotTriggered == 0x0) {
    }
  } 
}

void Utils_BusyMilliSleep(uint64_t useconds) {
  Utils_BusyMicroSleep(useconds * 1000);
}

void Utils_BusySecondSleep(uint64_t seconds) {
  Utils_BusyMicroSleep(seconds * 1000 * 1000);
}