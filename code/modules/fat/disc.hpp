#pragma once

#include <modules/fat/common.hpp>

typedef struct {
	const char* name; 
	const DISC_INTERFACE* (*getInterface)(void);
} INTERFACE_ID;

static inline bool _FAT_disc_isInserted (const DISC_INTERFACE* disc) {
	return disc->isInserted();
}

static inline bool _FAT_disc_readSectors (const DISC_INTERFACE* disc, sec_t sector, sec_t numSectors, void* buffer) {
	return disc->readSectors (sector, numSectors, buffer);
}

static inline bool _FAT_disc_writeSectors (const DISC_INTERFACE* disc, sec_t sector, sec_t numSectors, const void* buffer) {
	return disc->writeSectors (sector, numSectors, buffer);
}

static inline bool _FAT_disc_clearStatus (const DISC_INTERFACE* disc) {
	return disc->clearStatus();
}

static inline bool _FAT_disc_startup (const DISC_INTERFACE* disc) {
	return disc->startup();
}

static inline bool _FAT_disc_shutdown (const DISC_INTERFACE* disc) {
	return disc->shutdown();
}

static inline u32 _FAT_disc_hostType (const DISC_INTERFACE* disc) {
	return disc->ioType;
}
static inline u32 _FAT_disc_features (const DISC_INTERFACE* disc) {
	return disc->features;
}