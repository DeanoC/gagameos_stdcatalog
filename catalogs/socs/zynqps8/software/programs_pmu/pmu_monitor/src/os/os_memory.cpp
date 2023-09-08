#include "core/core.h"
#include "osservices/ipi3_transport.h"
#include "ipi3_os_server.hpp"
#include "../os_heap.hpp"

namespace IPI3_OsServer {

void DdrLoBlockAlloc(IPI_Channel senderChannel, IPI3_Msg const *msgBuffer) {
	IPI3_Response responseBuffer;
	uint32_t blocksWanted = msgBuffer->Payload.DdrLoBlockAlloc.blocks64KB;
	uint32_t tag = msgBuffer->Payload.DdrLoBlockFree.tag;

	debug_printf("Request for %lu KB from low memory %c%c%c%c\n", blocksWanted * 64,
	             (char)(tag >> 24), (char)(tag >> 16), (char)(tag >> 8), (char)(tag >> 0));

	uintptr_t address = osHeap->ddrLoAllocator.Alloc(blocksWanted);
	if (address == (uintptr_t) ~0) {
		responseBuffer.result = IRR_OUT_OF_MEMORY;
	} else {
//		debug_printf("SUCCESS %x\n", address);
//		osHeap->ddrLoAllocator.DebugDumpMasks(4);

		responseBuffer.result = IRR_SUCCESS;
		responseBuffer.DdrLoBlockAlloc.offset = address;
	}

	SubmitResponse(senderChannel, &responseBuffer);
}

void DdrLoBlockFree(IPI3_Msg const *msgBuffer) {
	uintptr_t const address = msgBuffer->Payload.DdrLoBlockFree.offset;
	uint16_t const blockCount = msgBuffer->Payload.DdrLoBlockFree.blockCount;
	uint32_t tag = msgBuffer->Payload.DdrLoBlockFree.tag;
//	osHeap->ddrLoAllocator.DebugDumpMasks(4);
	debug_printf("Free of low %d KB %x %c%c%c%c\n", blockCount * 64, address,
		(char)(tag >> 24), (char)(tag >> 16), (char)(tag >> 8), (char)(tag >> 0));

	osHeap->ddrLoAllocator.Free(address, blockCount);
}

void DdrHiBlockAlloc(IPI_Channel senderChannel, IPI3_Msg const *msgBuffer) {

	IPI3_Response responseBuffer;
	uint32_t blocksWanted = msgBuffer->Payload.DdrHiBlockAlloc.blocks64KB;
	uint32_t tag = msgBuffer->Payload.DdrHiBlockAlloc.tag;

	debug_printf("Request for %lu KB from hi memory %c%c%c%c\n", blocksWanted * 64,
	             (char)(tag >> 24), (char)(tag >> 16), (char)(tag >> 8), (char)(tag >> 0));

	uintptr_t address = osHeap->ddrHiAllocator.Alloc(blocksWanted);
	if (address == (uintptr_t) ~0) {
		responseBuffer.result = IRR_OUT_OF_MEMORY;
	} else {
		responseBuffer.result = IRR_SUCCESS;
		responseBuffer.DdrHiBlockAlloc.offset = address;
	}

	SubmitResponse(senderChannel, &responseBuffer);
}

void DdrHiBlockFree(IPI3_Msg const *msgBuffer) {
	uintptr_t const address = msgBuffer->Payload.DdrHiBlockFree.offset;
	uint16_t const blockCount = msgBuffer->Payload.DdrHiBlockFree.blockCount;
	uint32_t tag = msgBuffer->Payload.DdrHiBlockFree.tag;

	debug_printf("Free of hi %d KB %x %c%c%c%c\n", blockCount * 64, address,
	             (char)(tag >> 24), (char)(tag >> 16), (char)(tag >> 8), (char)(tag >> 0));
	osHeap->ddrHiAllocator.Free(address, blockCount);
}

void DdrLoStashAllocs(IPI3_Msg const *msgBuffer)
{
	debug_print("\nStashing DDR Lo allocations\n");
	if (osHeap->isDdrLoStashed) {
		debug_print("\nDDR Lo already stashed, existing stash will be deleted\n");
	}
	memcpy(osHeap->ddrLoSnapshot, &osHeap->ddrLoAllocator, sizeof(osHeap->ddrLoAllocator));
	osHeap->isDdrLoStashed = true;
}

void DdrLoRestoreAllocs(IPI3_Msg const *msgBuffer)
{
	debug_print("\nRestoring DDR Lo allocations to last stash\n");
	if (osHeap->isDdrLoStashed)
	{
		memcpy(&osHeap->ddrLoAllocator, osHeap->ddrLoSnapshot, sizeof(osHeap->ddrLoAllocator));
		osHeap->isDdrLoStashed = false;
	}
	else
	{
		debug_print("\n DDR Lo allocations not stashed\n");
	}
}

void DdrHiStashAllocs(IPI3_Msg const *msgBuffer)
{
	debug_print("\nStashing DDR Hi allocations\n");
	if (osHeap->isDdrHiStashed)
	{
		debug_print("\nDDR Hi already stashed, existing stash will be deleted\n");
	}
	memcpy(osHeap->ddrHiSnapshot, &osHeap->ddrHiAllocator, sizeof(osHeap->ddrHiAllocator));
	osHeap->isDdrHiStashed = true;
}

void DdrHiRestoreAllocs(IPI3_Msg const *msgBuffer)
{
	debug_print("\nRestoring DDR Hi allocations to last stash\n");
	if (osHeap->isDdrHiStashed)
	{
		memcpy(&osHeap->ddrHiAllocator, osHeap->ddrHiSnapshot, sizeof(osHeap->ddrHiAllocator));
		osHeap->isDdrHiStashed = false;
	}
	else
	{
		debug_print("\n DDR Hi allocations not stashed\n");
	}
}

} // end namespace