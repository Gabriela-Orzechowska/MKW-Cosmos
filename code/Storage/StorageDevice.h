#ifndef _STORAGEDEVICEH_
#define _STORAGEDEVICEH_

typedef unsigned int u32;
typedef int bool;

u32 StorageDevice_SectorSize__Fv();
bool StorageDevice_Read__FUiUiPv(u32 sector, u32 count, void * buffer);
bool StorageDevice_Write__FUiUiPCv(u32 sector, u32 count, const void * buffer);
bool StorageDevice_Erase__FUiUi(u32 sector, u32 count);
bool StorageDevice_Sync__Fv();
u32 StorageDevice_GetMessageId();

#endif