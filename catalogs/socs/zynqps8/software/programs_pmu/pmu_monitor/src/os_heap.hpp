#pragma once

#include "core/bitmap_allocator_single_threaded.hpp"
#include "text_console.hpp"
#include "host_interface.hpp"
#include "timers.hpp"
#include "osservices/osservices.h"

// the os heap is a 1MB chunk of DDR (at ddr base) that the pmu reserves
// for itself.
// if possible other CPUs should trap access to this to help catch NULLs
// it also ensure the data is safe if other CPU go mad
// The default MMU code for A53 only has 2MB pages so even though only 1MB
// so app should allocate a dummy 1MB or install a L3 MMU section

// its available once IPI3_OSServiceInit is finished
struct OsHeap {
	static void Init();
	[[maybe_unused]] static void Fini();

	static const unsigned int TotalSize = 1 * 1024*1024;
	static const unsigned int BounceBufferSize = 64 * 1024;
	static const unsigned int TmpOsBufferChunkSize = 64;
	static const unsigned int TmpOsBufferCount = 4096;
	static const unsigned int BootOCMStoreSize = 256 * 1024;

	const uint32_t nullBlock[1024]; // 4K poisoned to 0xDCDCDCDC for null page

	HostInterface hostInterface;
	BootData bootData;
  TextConsole console;

	bool isDdrLoStashed;
	bool isDdrHiStashed;
	bool screenConsoleEnabled;

	BitmapAllocator_SingleThreaded<64*1024, 2047*16> ddrLoAllocator;
	BitmapAllocator_SingleThreaded<64*1024, 2048*16> ddrHiAllocator;

	BitmapAllocator_SingleThreaded<TmpOsBufferChunkSize, TmpOsBufferCount> tmpOsBufferAllocator;

	uint8_t tmpOsBuffer[TmpOsBufferChunkSize * TmpOsBufferCount];
	uint8_t bounceBuffer[BounceBufferSize];
	uint8_t ddrLoSnapshot[sizeof(ddrLoAllocator)];
	uint8_t ddrHiSnapshot[sizeof(ddrHiAllocator)];
	uint8_t bootOCMStore[BootOCMStoreSize];

};

static_assert(sizeof(OsHeap) < (OsHeap::TotalSize));

enum class HundredHzTasks
{
	HOST_INPUT = 0,
	HOST_COMMANDS_PROCESSING
};

enum class ThirtyHzTasks
{
  TEXT_CONSOLE = 0
};

extern OsHeap *osHeap;