#include <kamek.hpp>
#include <game/UI/Page/Other/Title.hpp>
#include <game/UI/Page/Menu/MainMenu.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <core/System/RKSystem.hpp>
#include <game/Sound/AudioManager.hpp>
#include <main.hpp>

void null(){
    return;
}

#define CONGREGATION
#ifdef CONGREGATION

static int delay = 0;
static bool isTrolling = false;
void OnButtonPress(Pages::MainMenu * menu, PushButton * button) {
    RKSystem::sInstance->sceneManager->colorFader->setColor(0);
    RKSystem::sInstance->sceneManager->colorFader->fadeOut();
    menu->PlaySound(0xd0, 0xFFFFFFFF);

    AudioHandleHolder::sInstance->activeHandle->basicSound->Stop(50);
    isTrolling = true;
    delay = 0;
}
kmBranch(0x8085110c, OnButtonPress);

kmWrite32(0x8063bfcc, 0x4e800020);

void OnUpdate(Pages::MainMenu * menu){
    if(!isTrolling) return;
    if(RKSystem::sInstance->sceneManager->colorFader->status == Fader::CAN_FADE_IN){
        if(delay > 120){
            RKSystem::sInstance->sceneManager->colorFader->fadeIn();
            RKSystem::sInstance->sceneManager->colorFader->totalFadeDuration = 100;
            menu->AddPageLayer((PageId)0x59,0);
        }
    }
    delay++;
    if(delay > 1300){
        Cosmos::Panic(__FILE__, __LINE__, "April fools! :p\n");
    }
    

}
kmBranch(0x80851108, OnUpdate);
kmWrite32(0x8063bf4c, 0x38800002);
kmWrite32(0x806fa738, 0x4e800020);
kmWrite32(0x8063bd48, 0x38600000);
kmWrite32(0x8063bdc8, 0x38000001);

#endif