#include "core/core.h"
#include "dbg/print.h"
#include "dbg/assert.h"
#include "petitfatfs/petitfatfs.h"
#include "pff.h"

static_assert(sizeof(FATFS) == 44UL);
static_assert(sizeof(FATFS) == FATFS_SIZEOF_DRIVE);
extern FATFS* FatFs;

void FATFS_Mount(FATFS_DriveHandle drive_) {
	FRESULT mount_ok = pf_mount((FATFS *)drive_);
  if(mount_ok != FR_OK) {
    debug_printf("FATFS_Mount not OK {%u}", mount_ok);
  }
}

bool FATFS_Open(char const* filename_) {
	FRESULT result = pf_open(filename_);
	return (result == FR_OK);
}

void FATFS_Close() {
}

size_t FATFS_Read(void* buffer_, size_t byteCount_) {
	UINT bytesRead = 0;
	FRESULT result = pf_read(buffer_, byteCount_, &bytesRead);
	assert(result == FR_OK);

	return bytesRead;
}

bool FATFS_Seek(int32_t offset_, enum FATFS_FileSeekDir origin_) {
	FRESULT result = FR_OK;
	switch (origin_) {
	case FATFS_FSD_Begin:
		result = pf_lseek(offset_);
		return (result == FR_OK);
	case FATFS_FSD_Current:
		result = pf_lseek(FatFs->fptr + offset_);
		return (result == FR_OK);
	case FATFS_FSD_End:
		result = pf_lseek(FatFs->fsize - offset_);
		return (result == FR_OK);
	default: return false;
	}
}

int32_t FATFS_Tell() {
	return FatFs->fptr;
}

size_t FATFS_Size() {
	return FatFs->fsize;
}

bool FATFS_IsEOF() {
	return FatFs->fsize == FatFs->fptr;
}