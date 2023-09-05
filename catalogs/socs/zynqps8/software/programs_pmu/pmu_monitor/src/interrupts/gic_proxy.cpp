#include "core/core.h"
#include "gic_proxy.hpp"
#include "platform/memory_map.h"
#include "platform/reg_access.h"
#include "platform/registers/lpd_slcr.h"
#include "platform/registers/uart.h"
#include "platform/registers/ttc.h"
#include "dbg/raw_print.h"
#include "../os_heap.hpp"

uint32_t uartDebugReceiveLast;
uint32_t uartDebugReceiveHead;
static const unsigned int UartRecvBufferSize = 1 * 1024;
uint8_t uartDEBUGReceiveBuffer[UartRecvBufferSize]; // filled in an interrupt!

#define UART_DEBUG_BASE_ADDR UART1_BASE_ADDR

#define IsTransmitFull() (HW_REG_GET_BIT(HW_REG_GET_ADDRESS(UART_DEBUG), UART, CHANNEL_STS, TNFUL))
#define IsReceiveEmpty() (HW_REG_GET_BIT(HW_REG_GET_ADDRESS(UART_DEBUG), UART, CHANNEL_STS, REMPTY))

extern "C" void UartPutSizedData(uint32_t size, uint8_t const * text)
{
	for(uint32_t index = 0; index < size; index++)
	{
		// block 
		while (IsTransmitFull())
		{
		}

		HW_REG_WRITE(HW_REG_GET_ADDRESS(UART_DEBUG), UART, TX_RX_FIFO, text[index]);
		// expand newline inline (TODO remove this for binary sends)
		if (text[index] == '\n')
		{
			// block 
			while (IsTransmitFull())
			{
			}
			HW_REG_WRITE(HW_REG_GET_ADDRESS(UART_DEBUG), UART, TX_RX_FIFO, '\r');
		}
	}
}

void UartTmpBufferRefill(uintptr_t &tmpBufferAddr_, uint32_t &tmpBufferSize_)
{
	if (uartDebugReceiveLast != uartDebugReceiveHead)
	{
		uint32_t last = uartDebugReceiveLast;
		uint32_t head = uartDebugReceiveHead;

		if (last > head)
		{
			// wrapped
			auto firstSize = UartRecvBufferSize - last;
			tmpBufferSize_ = firstSize + head;
			tmpBufferAddr_ = osHeap->tmpOsBufferAllocator.Alloc(BitOp::PowerOfTwoContaining(tmpBufferSize_ / 64));
			memcpy((char *)tmpBufferAddr_, &uartDEBUGReceiveBuffer[last], firstSize);
			memcpy((char *)tmpBufferAddr_ + firstSize, &uartDEBUGReceiveBuffer[0], head);
		}
		else
		{
			tmpBufferSize_ = head - last;
			tmpBufferAddr_ = osHeap->tmpOsBufferAllocator.Alloc(BitOp::PowerOfTwoContaining(tmpBufferSize_ / 64));
			memcpy((char *)tmpBufferAddr_, &uartDEBUGReceiveBuffer[last], tmpBufferSize_);
		}
		uartDebugReceiveLast = head;
	}
	else
	{
		tmpBufferAddr_ = 0;
		tmpBufferSize_ = 0;
	}
}

static void UART_DEBUG_Interrupt() {
	uint32_t status = HW_REG_READ(HW_REG_GET_ADDRESS(UART_DEBUG), UART, CHNL_INT_STS) &
		HW_REG_READ(HW_REG_GET_ADDRESS(UART_DEBUG), UART, INTRPT_MASK);
	// disabled UART interrupts
	HW_REG_WRITE(HW_REG_GET_ADDRESS(UART_DEBUG), UART, INTRPT_DIS, status);
	HW_REG_WRITE(HW_REG_GET_ADDRESS(UART_DEBUG), UART, CHNL_INT_STS, status);

	if(status & UART_CHNL_INT_STS_RFUL || status & UART_CHNL_INT_STS_TIMEOUT) {
		// receive FIFO is full, copy fifo out to buffer
		while(!IsReceiveEmpty()) {
			uint32_t head = uartDebugReceiveHead + 1;
			uartDebugReceiveHead = (head >= UartRecvBufferSize) ? 0 : head;
			uartDEBUGReceiveBuffer[head - 1] = HW_REG_READ(HW_REG_GET_ADDRESS(UART_DEBUG), UART, TX_RX_FIFO);
		}
	}
	if(status & UART_CHNL_INT_STS_REMPTY) {
		status |= UART_INTRPT_DIS_RTRIG;
		status &= ~(UART_INTRPT_EN_TIMEOUT | UART_CHNL_INT_STS_RFUL | UART_CHNL_INT_STS_REMPTY);
	}

	if(status & UART_CHNL_INT_STS_RTRIG) {
		status |= (UART_INTRPT_EN_TIMEOUT | UART_CHNL_INT_STS_RFUL | UART_CHNL_INT_STS_REMPTY);
		status &= ~UART_INTRPT_DIS_RTRIG;
	}

	HW_REG_WRITE(HW_REG_GET_ADDRESS(UART_DEBUG), UART, INTRPT_EN, status); // enable interrupts
}

void GIC_Proxy(void) {
	const uint32_t maxRestartCount = 10;
	uint32_t restartCount = 0;

restart:;
	const uint32_t gicpIrqStatus = HW_REG_READ1(LPD_SLCR, GICP_PMU_IRQ_STATUS);

	for(uint32_t group = 0; group < 5; group++) {
		const uint32_t bit = 1 << group;
		if(bit & gicpIrqStatus) {
			const volatile uint32_t gicbits =
					hw_RegRead(LPD_SLCR_BASE_ADDR,LPD_SLCR_GICP0_IRQ_STATUS_OFFSET + (group * GICPROXY_INTERRUPT_GROUP_SIZE));

//			raw_debug_printf("group 0x%lx gicbits 0x%lx\n", group, gicbits);

			const Interrupt_Names name = GIC_ProxyToName(group, gicbits);
			switch(name) {
#if UART_DEBUG_BASE_ADDR == 0xff010000
				case INT_UART1: UART_DEBUG_Interrupt(); break;
#else
				case INT_UART0: UART_DEBUG_Interrupt(); break;
#endif
				default:
					break;
			}
			hw_RegWrite(LPD_SLCR_BASE_ADDR, LPD_SLCR_GICP0_IRQ_STATUS_OFFSET + (group * GICPROXY_INTERRUPT_GROUP_SIZE), gicbits);
			HW_REG_WRITE1(LPD_SLCR, GICP_PMU_IRQ_STATUS, bit);

			if(HW_REG_READ1(LPD_SLCR, GICP_PMU_IRQ_STATUS) & bit) {
				if(restartCount < maxRestartCount) {
					restartCount++;
					goto restart;
				} else {
					raw_debug_printf("GIC %d is running too fast, can't keep up!\n", name);
				}
			}
		}
	}


}
