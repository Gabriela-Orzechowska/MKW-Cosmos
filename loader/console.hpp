#pragma once
#include <functions.hpp>

typedef s32 (*AsyncDisplay_endRender_t)(void*);
static AsyncDisplay_endRender_t AsyncDisplay_endRender = nullptr;


void Console_Render(void* display);
void Console_Print(const char* msg);

void Console_Init(loaderFunctions* funcs);
void Console_Destroy();
void Console_Clear();
