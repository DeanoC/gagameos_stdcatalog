#pragma once
#include "core/core.h"

enum FATFS_FileSeekDir {
	FATFS_FSD_Begin = 0,
	FATFS_FSD_Current,
	FATFS_FSD_End,
};

#define FATFS_SIZEOF_DRIVE (44UL)
typedef struct FATFS* FATFS_DriveHandle;

#define FATFS_DECLARE_DRIVE(name) \
uint8_t name##_MEMORY[FATFS_SIZEOF_DRIVE]; \
FATFS_DriveHandle name = (FATFS_DriveHandle) name##_MEMORY;

EXTERN_C void FATFS_Mount(FATFS_DriveHandle drive_);
EXTERN_C NON_NULL(1) bool FATFS_Open(char const* filename_);
EXTERN_C NON_NULL(1) size_t FATFS_Read(void* buffer_, size_t byteCount_);
EXTERN_C bool FATFS_Seek(int32_t offset, enum FATFS_FileSeekDir origin_);
EXTERN_C int32_t FATFS_Tell();
EXTERN_C size_t FATFS_Size();
EXTERN_C bool FATFS_IsEOF();
