/*
 * dma_hp_sg.c
 *
 *  Created on: 1 Apr 2026
 *      Author: Bekdouche
 */

#include "xaxidma.h"
#include "xparameters.h"
#include "xdebug.h"

#ifdef XPAR_PS7_RAM_0_S_AXI_BASEADDR
#define DDR_BASE_ADDR		XPAR_PS7_RAM_0_S_AXI_BASEADDR
#define MEM_BASE_ADDR		(DDR_BASE_ADDR + 0x10000000)
#endif

#define TX_BD_SPACE_BASE	MEM_BASE_ADDR
#define RX_BD_SPACE_BASE	(MEM_BASE_ADDR + 0x00001000)
#define TX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00002000)
#define RX_BUFFER_BASE		(MEM_BASE_ADDR + 0x00003000)

#define BD_SIZE 0x40
#define PKT_LEN 0x20
#define BD_NUM 6
#define TEST_START_VALUE 0xC

u32 *DMA = XPAR_AXI_DMA_0_BASEADDR;

int main (void)
{
	u32 *TxBDPtr = (u32 *) TX_BD_SPACE_BASE;
	u32 *RxBDPtr = (u32 *) RX_BD_SPACE_BASE;
	u32 *TxBuffPtr = (u32 *) TX_BUFFER_BASE;
	u32 *RxBuffPtr = (u32 *) RX_BUFFER_BASE;

	u32 TxBDadd = TX_BD_SPACE_BASE;
	u32 RxBDadd = RX_BD_SPACE_BASE;
	u32 TxBuffAdd = TX_BUFFER_BASE;
	u32 RxBuffAdd = RX_BUFFER_BASE;

	u32 tmp0, tmp1;

	int value;

	u8 *PktPtr = (u8 *) TX_BUFFER_BASE;

	// generate values
	for (int i = 0; i < PKT_LEN*BD_NUM; i++)
	{
		PktPtr[i] = value;
		value++;
	}

	// flush cache
	Xil_DCacheFlushRange((UINTPTR)PktPtr, PKT_LEN*BD_NUM);

	// generate the two list for the sgdma
	for (int i = 0; i<BD_NUM; i++)
	{
		TxBDadd += BD_SIZE;
		RxBDadd += BD_SIZE;

		// tx block distributor registers
		TxBDPtr[((i*BD_SIZE)+ 0x00)/4] = TxBDadd; 				// next distributor pointer
		TxBDPtr[((i*BD_SIZE)+ 0x04)/4] = 0x00000000; 			// N/A
		TxBDPtr[((i*BD_SIZE)+ 0x08)/4] = TxBuffAdd; 			// buffer address
		TxBDPtr[((i*BD_SIZE)+ 0x0C)/4] = 0x00000000; 			// N/A
		TxBDPtr[((i*BD_SIZE)+ 0x10)/4] = 0x00000000; 			// N/A
		TxBDPtr[((i*BD_SIZE)+ 0x14)/4] = 0x00000000; 			// N/A
		TxBDPtr[((i*BD_SIZE)+ 0x18)/4] = 0x00000000 + PKT_LEN; 	// control (number of bytes)
		TxBDPtr[((i*BD_SIZE)+ 0x1C)/4] = 0x00000000; 			// status (must be initialised to 0)
		TxBDPtr[((i*BD_SIZE)+ 0x20)/4] = 0x00000000; 			// usr app field 0
		TxBDPtr[((i*BD_SIZE)+ 0x24)/4] = 0x00000000; 			// usr app field 1
		TxBDPtr[((i*BD_SIZE)+ 0x28)/4] = 0x00000000; 			// usr app field 2
		TxBDPtr[((i*BD_SIZE)+ 0x2C)/4] = 0x00000000; 			// usr app field 3
		TxBDPtr[((i*BD_SIZE)+ 0x30)/4] = 0x00000000; 			// usr app field 4
		TxBDPtr[((i*BD_SIZE)+ 0x34)/4] = 0x00000000; 			// not used
		TxBDPtr[((i*BD_SIZE)+ 0x38)/4] = 0x00000000; 			// not used
		TxBDPtr[((i*BD_SIZE)+ 0x3C)/4] = 0x00000000; 			// not used

		// rx block distributor registers
		RxBDPtr[((i*BD_SIZE)+ 0x00)/4] = RxBDadd; 				// next descriptor pointer
		RxBDPtr[((i*BD_SIZE)+ 0x04)/4] = 0x00000000; 			// N/A
		RxBDPtr[((i*BD_SIZE)+ 0x08)/4] = RxBuffAdd; 			// buffer address
		RxBDPtr[((i*BD_SIZE)+ 0x0C)/4] = 0x00000000; 			// N/A
		RxBDPtr[((i*BD_SIZE)+ 0x10)/4] = 0x00000000; 			// N/A
		RxBDPtr[((i*BD_SIZE)+ 0x14)/4] = 0x00000000; 			// N/A
		RxBDPtr[((i*BD_SIZE)+ 0x18)/4] = 0x00000000 + PKT_LEN; 	// control (number of bytes)
		RxBDPtr[((i*BD_SIZE)+ 0x1C)/4] = 0x00000000; 			// status (must be initialised to 0)
		RxBDPtr[((i*BD_SIZE)+ 0x20)/4] = 0x00000000; 			// usr app field 0
		RxBDPtr[((i*BD_SIZE)+ 0x24)/4] = 0x00000000; 			// usr app field 1
		RxBDPtr[((i*BD_SIZE)+ 0x28)/4] = 0x00000000; 			// usr app field 2
		RxBDPtr[((i*BD_SIZE)+ 0x2C)/4] = 0x00000000; 			// usr app field 3
		RxBDPtr[((i*BD_SIZE)+ 0x30)/4] = 0x00000000; 			// usr app field 4
		RxBDPtr[((i*BD_SIZE)+ 0x34)/4] = 0x00000000; 			// not used
		RxBDPtr[((i*BD_SIZE)+ 0x38)/4] = 0x00000000; 			// not used
		RxBDPtr[((i*BD_SIZE)+ 0x3C)/4] = 0x00000000; 			// not used

		// increase buffers
		TxBuffAdd += PKT_LEN;
		RxBuffAdd += PKT_LEN;
	}

	// flush cache
	Xil_DCacheFlushRange((UINTPTR)TxBDPtr, BD_SIZE*BD_NUM);
	Xil_DCacheFlushRange((UINTPTR)RxBDPtr, BD_SIZE*BD_NUM);

	// axi dma scatter-gather registers
	DMA[0x00/4] = 0x04; // mm2s control register (generate reset)
	DMA[0x30/4] = 0x04; // s2mm control register (generate reset)

	DMA[0x08/4] = TX_BD_SPACE_BASE; // mm2s current descriptor pointer register
	DMA[0x38/4] = RX_BD_SPACE_BASE; // s2mm current descriptor pointer register

	tmp0 = DMA[0x00/4]; // get mm2s control register value
	DMA[0x00/4] = tmp0 | 0x00000001; // start mm2s channel
	tmp1 = DMA[0x30/4]; // get s2mm control register value
	DMA[0x30/4] = tmp1 | 0x00000001; // start s2mm channel

	// start with Rx before the Tx to make sure the Rx is ready to receive the data when the Tx starts sending it
	DMA[0x40/4] = RX_BD_SPACE_BASE + (BD_SIZE * BD_NUM) - BD_SIZE; // s2mm tail descriptor pointer register (point to the last descriptor in the list)
	DMA[0x10/4] = TX_BD_SPACE_BASE + (BD_SIZE * BD_NUM) - BD_SIZE; // mm2s tail descriptor pointer register (point to the last descriptor in the list)

	// check status registers to see if the transfer is done
	tmp0 = 0;
	tmp1 = 0;
	// second bit of the status register is the "idle" bit, which is set to 1 when the transfer is done
	while ((tmp0 & 0x00000002) == 0 || (tmp1 & 0x00000002) == 0)
	{
		tmp0 = DMA[0x04/4]; // mm2s status register
		tmp1 = DMA[0x34/4]; // s2mm status register
	}

	Xil_DCacheInvalidateRange((UINTPTR)TxBDPtr, BD_SIZE*BD_NUM);
	Xil_DCacheInvalidateRange((UINTPTR)RxBDPtr, BD_SIZE*BD_NUM);
	Xil_DCacheInvalidateRange((UINTPTR)TxBuffPtr, PKT_LEN*BD_NUM);
	Xil_DCacheInvalidateRange((UINTPTR)RxBuffPtr, PKT_LEN*BD_NUM);
	return XST_SUCCESS;
}
