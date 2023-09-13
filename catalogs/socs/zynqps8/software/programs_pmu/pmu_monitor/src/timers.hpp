#pragma once

extern "C" void Timers_OneShotTimer(uint32_t microseconds_);
extern "C" volatile uint32_t Timers_OneShotTriggered;

namespace Timers {
  static const int MaxHundredHzCallbacks = 4;
	static const int MaxThirtyHzCallbacks = 4;

	typedef void (*Callback)();

	void Init();
	void Start();
}

