#pragma once
#include <kamek.hpp>

bool WiiLinkHasFinished();

void ReportToWiiLink(const char* key, const char* string);
void ReportToWiiLinkB64(const char* key, const void* data, u32 size);
void ReportToWiiLinkU32(const char* key, u32 val);
