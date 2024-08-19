#include <console.hpp>

#define CONSOLE_BUFFER_SIZE 1200

static char* consoleBuffer = (char*) 0x91000000;
static int consoleCursor = 0;

loaderFunctions* loaderFuncs = nullptr;
DisplayFunctions* displayFuncs = nullptr;

void Console_Render(void* display){
    void* buffer = displayFuncs->VIGetNextFrameBuffer();
    if(buffer == nullptr) {
        AsyncDisplay_endRender(display);
        return;
    }
    displayFuncs->ChangeXfb(buffer, 608, 456);
    displayFuncs->DrawString(0,0,consoleBuffer,0,0);
    displayFuncs->StoreCache();

    AsyncDisplay_endRender(display);
}
void Console_Init(loaderFunctions* func) {
    
    displayFuncs = displayFunctionsSets[GetRegionIndex()];
    loaderFuncs = func;

    AsyncDisplay_endRender = (AsyncDisplay_endRender_t) *((u32*)displayFuncs->endRenderAddress);
    *((u32*)displayFuncs->endRenderAddress) = (u32)&Console_Render;
    consoleCursor = 0;
}

void Console_Print(const char* msg) {
    if(consoleCursor == CONSOLE_BUFFER_SIZE) return;
    for(;*msg; msg++){
        consoleBuffer[consoleCursor++] = *msg;
        if(consoleCursor == CONSOLE_BUFFER_SIZE) return;
    }
    consoleBuffer[consoleCursor] = '\0';
    if(!loaderFuncs->rkSystem->asyncDisplay) return;
    loaderFuncs->rkSystem->asyncDisplay->endFrame();
    loaderFuncs->rkSystem->asyncDisplay->beginFrame();
}

void Console_Clear(){
    consoleCursor = 0;
    consoleBuffer[0] = 0;
}

void Console_Destroy(){
    for(int i = 0; i < consoleCursor; ){
        consoleBuffer[i] = 0;
        i++;
    }
    *((u32*)displayFuncs->endRenderAddress) = (u32) AsyncDisplay_endRender;
}
