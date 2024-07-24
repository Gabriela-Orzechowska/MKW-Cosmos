#include <console.hpp>

#define CONSOLE_BUFFER_SIZE 600

static char consoleBuffer[CONSOLE_BUFFER_SIZE] = {0};
static int consoleCursor = 0;

loaderFunctions* console_funcs = nullptr;

void Console_Render(void* display){
    void* buffer = console_funcs->VIGetNextFrameBuffer();
    if(buffer == nullptr) {
        AsyncDisplay_endRender(display);
        return;
    }
    console_funcs->DirentPrint_ChangeXFB(buffer, 608, 456);
    console_funcs->DirectPrint_DrawString(0,0,consoleBuffer,0,0);
    console_funcs->DirectPrint_StoreCache();

    AsyncDisplay_endRender(display);
}
void Console_Init(loaderFunctions* func) {
    console_funcs = func; 
    AsyncDisplay_endRender = (AsyncDisplay_endRender_t) *((u32*)console_funcs->endRenderAddress);
    *((u32*)console_funcs->endRenderAddress) = (u32)&Console_Render;
    consoleCursor = 0;
}

void Console_Print(const char* msg) {
    if(consoleCursor == CONSOLE_BUFFER_SIZE) return;
    for(;*msg; msg++){
        consoleBuffer[consoleCursor++] = *msg;
        if(consoleCursor == CONSOLE_BUFFER_SIZE) return;
    }
    consoleBuffer[consoleCursor] = '\0';
    if(!console_funcs->rkSystem->asyncDisplay) return;
    console_funcs->rkSystem->asyncDisplay->endFrame();
    console_funcs->rkSystem->asyncDisplay->beginFrame();
}

void Console_Clear(){
    consoleCursor = 0;
    consoleBuffer[0] = 0;
}

void Console_Destroy(){
    *((u32*)console_funcs->endRenderAddress) = (u32) AsyncDisplay_endRender;
}
