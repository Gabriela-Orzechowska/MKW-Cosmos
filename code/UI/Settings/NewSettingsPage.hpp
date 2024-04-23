#pragma once
#include <kamek.hpp>
#include <game/UI/Page/Page.hpp>
#include <game/UI/Ctrl/UpDown.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <Settings/UserData.hpp>
#include <UI/BMG/BMG.hpp>


namespace CosmosUI
{
    class NewSettings : public Page {
    public:
        NewSettings();
        ~NewSettings() override {}

        void OnInit() override;
        void OnActivate() override;
        void BeforeEntranceAnimations() override;
        void OnUpdate() override;
        PageId GetNextPage() const override { return this->returnPage; }

        static void SetPreviousPageGlobal(PageId id, MenuId menu){
            NewSettings* page = MenuData::GetStaticInstance()->curScene->Get<NewSettings>((PageId)Cosmos::SETTINGS_MAIN);
            if(page != nullptr){
                page->SetPreviousPage(id, menu);
            }
        }
        void SetPreviousPage(PageId id, MenuId menu) { this->returnPage = id; this->returnMenu = menu; }
        MenuId GetPreviousMenu() const { return this->returnMenu; }

    private:
        void OnBack(u32 hudSlotId);
        void OnBackButtonClick(PushButton * button, u32 hudSlotId);

        void OnSettingsPageControlChange(TextUpDownValueControl::TextControl* valueControl, u32 optionId);
        void OnSettingsPageControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnSettingsPageControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnValueControlChange(UpDownControl* upDownControl, u32 hudSlotId, u32 index);
        void OnValueControlClick(UpDownControl* upDownControl, u32 hudSlotId);
        void OnValueControlSelect(UpDownControl* upDownControl, u32 hudSlotId);

        void OnDummyDeselect(UpDownControl*,u32) {}

        void ChangePage(u8 page);

        ControlsManipulatorManager controlsManipulatorManager; //0x48
        CtrlMenuBackButton backButton; //0x268
    
        UpDownControl pageSelector; //0x4CC
        TextUpDownValueControl textPageSelector; //0xA94
        UpDownControl settingSelectors[6]; // 0xF08
        TextUpDownValueControl textSettingSelector[6]; //0x31B8

        CtrlMenuPageTitleText title;
        CtrlMenuInstructionText bottomText;

        PtmfHolder_1A<Page, void, u32> onBackPressHandler;       
        PtmfHolder_2A<Page, void, PushButton*, u32> onBackButtonPress; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onPageClickHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onPageSelectHandler; 
        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onPageChangeHandler; 

        PtmfHolder_2A<Page, void, UpDownControl*, u32> onValueSettingClickHandler;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onValueSettingSelectHandler;
        PtmfHolder_3A<Page, void, UpDownControl*, u32, u32> onValueSettingChangeHandler;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onDeselectHandler;

        u8 currentPage;
        PageId returnPage;
        MenuId returnMenu;
    };
} // namespace CosmosUI
