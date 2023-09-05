#include "core/core.h"
#include "core/snprintf.h"
#include "platform/memory_map.h"
#include "platform/reg_access.h"
#include "platform/registers/uart.h"
#include "osservices/ipi3_transport.h"
#include "ipi3_os_server.hpp"
#include "../os_heap.hpp"
#include "osservices/osservices.h"
#include "utils/string_utils.hpp"

extern "C" void UartPutSizedData(uint32_t size, const uint8_t *text);

namespace IPI3_OsServer
{
void DebugInlinePrint(IPI3_Msg const *msgBuffer) {
	uint8_t size = msgBuffer->Payload.InlinePrint.size;

	const auto *text = (const uint8_t *) msgBuffer->Payload.InlinePrint.text;

	// TODO allocate a tmpBuffer for each print and send each one to the uart outside the ipi interrupt using a main callback
	//	tmpBufferAddr_ = osHeap->tmpOsBufferAllocator.Alloc(BitOp::PowerOfTwoContaining(size / 64));
	//	memcpy((char *)tmpBufferAddr_, text, size);

	UartPutSizedData(size, text);
}

void DebugPtrPrint(IPI_Channel senderChannel, IPI3_Msg const *msgBuffer) {
	uint32_t size = msgBuffer->Payload.DdrPacket.packetSize - IPI3_HEADER_SIZE - sizeof(IPI3_DdrPacket);
	const auto *text = (const uint8_t *) (msgBuffer->Payload.PtrPrint.text);
	UartPutSizedData(size, text);
}

} // end namespace

// override the weak prints, to write directly into the buffer
EXTERN_C WEAK_LINKAGE void OsService_InlinePrint(uint8_t size, const char *const text) {
	UartPutSizedData(size, (const uint8_t *)text);
}

EXTERN_C WEAK_LINKAGE void OsService_Print(const char *const text) {
	OsService_PrintWithSize(Utils_StringLength(text), text);
}

EXTERN_C WEAK_LINKAGE void OsService_PrintWithSize(unsigned int count, const char *const text) {
	UartPutSizedData(count, (const uint8_t *)text);
}

EXTERN_C WEAK_LINKAGE void OsService_Printf(const char *format, ...) {
	char buffer[256]; // 256 byte max string (on stack)
	va_list va;
	va_start(va, format);
	int const len = vsnprintf(buffer, 256, format, va);
	va_end(va);
	buffer[255] = 0;

	OsService_PrintWithSize(len,buffer);
}

EXTERN_C WEAK_LINKAGE void debug_print(char const * text){
	OsService_Print(text);
}
EXTERN_C WEAK_LINKAGE void debug_printf(const char *format, ...) {
	char buffer[256]; // 256 byte max string (on stack)
	va_list va;
	va_start(va, format);
	int const len = vsnprintf(buffer, 256, format, va);
	va_end(va);
	buffer[255] = 0;

	OsService_PrintWithSize(len,buffer);
}
EXTERN_C WEAK_LINKAGE void debug_sized_print(uint32_t size, char const * text) {
	OsService_PrintWithSize(size, text);
}
