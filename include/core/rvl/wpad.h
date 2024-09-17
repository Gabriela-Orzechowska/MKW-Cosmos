#ifndef _WPAD_
#define _WPAD_

#include "types.hpp"
#include "Input/InputData.hpp"

s32 WPADProbe(s32 channel, u32 * controllerType); //801c0990
void KPADRead(s32 channel,  void * status, u32 lenght); //80197380
void PADRead(PADStatus* status); //801af44c
void KPADGetUnifiedWpadStatus(s32 channel,  void * status, u32 lenght); //8019812c

#endif
