/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "core/core.h"
#include "dbg/print.h"
#include "../pff.h"			/* Obtains integer types */
#include "diskio.h" /* Declarations of disk functions */
#include "library_defines/library_defines.h"

#if IKUY_HAVE_LIB_ZYNQPS8_XILINX
#include "sdcard.h"
#else
#error IKUY_HAVE_LIB_ZYNQPS8_XILINX required
#endif

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
// Physical drive nmuber to identify the drive
DSTATUS disk_initialize() {
#if IKUY_HAVE_LIB_ZYNQPS8_XILINX
	if (FATFS_ZynqMPSDCardInit()) {
		return 0;
	}
#endif
	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_readp(
	BYTE* buff,		/* Data buffer to store read data */
	DWORD sector, /* Sector number (LBA) */
	UINT offser, 	/* Offset in the sector */
	UINT count		/* Byte count (bit15:destination) */
) {
#if IKUY_HAVE_LIB_ZYNQPS8_XILINX
	BYTE sector_buffer[512];

	if (FATFS_ZynqMPSDCardRead(sector, sector_buffer, 1)) {
		memcpy(buff, sector_buffer + offser, count);
		return RES_OK;
	}
#endif
	return RES_ERROR;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if PF_USE_WRITE

DRESULT disk_writep(
	const BYTE* buff, /* Data to be written */
	DWORD sc		/* Sector number (LBA) or Number of bytes to send */
) {
	DRESULT res = RES_ERROR;
	if (!buff) {
		if (sc) {
			// Initiate write process
		} else {
			// Finalize write process
		}
	} else {
		// Send data to the disk
#if IKUY_HAVE_LIB_ZYNQPS8_XILINX
		if (FATFS_ZynqMPSDCardWrite(sector, buff, sc)) {
			return RES_OK;
		}
#endif
	}

	return res;
	
}

#endif
