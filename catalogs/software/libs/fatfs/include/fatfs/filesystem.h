#pragma once
#include "core/core.h"
#include "memory/memory.h"
#include "core/utf8.h"
#include "fatfs/fatfs.h"

EXTERN_C NON_NULL(1) bool FATFS_MakeDirectory(utf8_int8_t const* dirpath_);
EXTERN_C NON_NULL(1) bool FATFS_Delete(utf8_int8_t const* path_);
EXTERN_C NON_NULL(1, 2) bool FATFS_Rename(utf8_int8_t const* oldpath_, utf8_int8_t const* newpath_);

typedef struct FATFS_DirectoryEnumerator* FATFS_DirectoryEnumeratorHandle;
typedef struct FATFS_DirectoryEnumeratorItem {
	utf8_int8_t const* filename;
	bool directory;
} FATFS_DirectoryEnumeratorItem;

#define FATFS_SIZEOF_DIRECTORYENUMERATOR (88 + sizeof(FATFS_DirectoryEnumeratorItem) + 288)

#define FATFS_DECLARE_DIRECTORYENUMERATOR(name) \
	uint8_t name##_MEMORY[FATFS_SIZEOF_DRIVE];    \
	FATFS_DirectoryEnumeratorHand name = (FATFS_DirectoryEnumeratorHandle)name##_MEMORY;

EXTERN_C NON_NULL(1, 2) bool FATFS_DirectoryEnumeratorCreate(FATFS_DirectoryEnumeratorHandle dir_, utf8_int8_t const* path_);
EXTERN_C NON_NULL(1) void FATFS_DirectoryEnumeratorDestroy(FATFS_DirectoryEnumeratorHandle handle);
EXTERN_C NON_NULL(1) FATFS_DirectoryEnumeratorItem const* FATFS_DirectoryEnumeratorNext(FATFS_DirectoryEnumeratorHandle handle);
