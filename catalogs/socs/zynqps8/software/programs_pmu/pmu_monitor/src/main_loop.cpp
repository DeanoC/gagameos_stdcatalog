#include "core/core.h"
#include "main_loop.hpp"
#include "os_heap.hpp"

void MainLoop::Init() {
	endLoop = false;
	thirtyHertzTrigger = false;
	hundredHertzTrigger = false;
	osHeap->hostInterface.Init();
}

[[maybe_unused]] void MainLoop::Fini() {
	osHeap->hostInterface.Fini();
}

Timers::Callback hundredHzCallbacks[Timers::MaxHundredHzCallbacks];
Timers::Callback thirtyHzCallbacks[Timers::MaxThirtyHzCallbacks];

void MainLoop::Loop() {
	while(!this->endLoop) {
		if(this->hundredHertzTrigger) {
			this->hundredHertzTrigger = false;
			for(int i=0;i < Timers::MaxHundredHzCallbacks;++i) {
				auto callback = hundredHzCallbacks[i];
				if (callback != nullptr) {
					callback();
				}
			}
		}
		if(this->thirtyHertzTrigger) {
			this->thirtyHertzTrigger = false;
			for(int i=0;i < Timers::MaxThirtyHzCallbacks;++i) {
				auto callback = thirtyHzCallbacks[i];
				if (callback != nullptr) {
					callback();
				}
			}
		}
	}
	debug_print("MainLoop ending\n");
}