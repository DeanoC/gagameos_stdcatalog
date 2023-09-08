#pragma once

#include "osservices/ipi3_transport.h"
namespace IPI3_OsServer
{

void Init();
void Handler(IPI_Channel senderChannel);
void SubmitResponse(IPI_Channel senderChannel, const IPI3_Response* const response);

void DebugInlinePrint(const IPI3_Msg* msgBuffer) NON_NULL(1);
void DebugPtrPrint(IPI_Channel senderChannel, const IPI3_Msg* msgBuffer) NON_NULL(2);

void DdrLoBlockAlloc(IPI_Channel senderChannel, const IPI3_Msg* msgBuffer) NON_NULL(2);
void DdrLoBlockFree(const IPI3_Msg* msgBuffer) NON_NULL(1);
void DdrHiBlockAlloc(IPI_Channel senderChannel, const IPI3_Msg* msgBuffer) NON_NULL(2);
void DdrHiBlockFree(const IPI3_Msg* msgBuffer) NON_NULL(1);
void DdrLoStashAllocs(IPI3_Msg const* msgBuffer) NON_NULL(1);
void DdrLoRestoreAllocs(IPI3_Msg const* msgBuffer) NON_NULL(1);
void DdrHiStashAllocs(IPI3_Msg const* msgBuffer) NON_NULL(1);
void DdrHiRestoreAllocs(IPI3_Msg const* msgBuffer) NON_NULL(1);

void BootComplete(const IPI3_Msg* msgBuffer) NON_NULL(1);
void FetchBootData(IPI_Channel senderChannel, const IPI3_Msg* msgBuffer) NON_NULL(2);
void CpuWakeOrSleep(IPI3_Msg const* msgBuffer) NON_NULL(1);

void ScreenConsoleEnable(const IPI3_Msg *msgBuffer) NON_NULL(1);
void ScreenConsoleInlinePrint(const IPI3_Msg *msgBuffer) NON_NULL(1);
void ScreenConsolePtrPrint(IPI_Channel senderChannel, const IPI3_Msg *msgBuffer) NON_NULL(2);

} // end namespace
