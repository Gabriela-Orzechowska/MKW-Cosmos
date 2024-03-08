
#ifndef _COSMOS_SETTINGSBASEPAGE_
#define _COSMOS_SETTINGSBASEPAGE_
#pragma once

#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <game/UI/Page/Menu/Menu.hpp>
#include <game/UI/Page/Menu/VSSettings.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuText.hpp>
#include <Settings/UserData.hpp>
#include <main.hpp>


extern u32 VSSettings_sInstance;

namespace CosmosUI
{
    class SettingsBasePage : public Pages::Menu
    {
        public:
        SettingsBasePage();
        ~SettingsBasePage() override;
        void OnInit() override;
        void OnActivate() override;
        int GetRuntimeTypeInfo() const override;
        int GetActivePlayerBitfield() const override;
        int GetPlayerBitfield() const override;
        ManipulatorManager *GetManipulatorManager() override;
        UIControl *CreateExternalControl(u32 id) override;
        void OnExternalButtonSelect(PushButton  *button, u32 param_2) override;
        UIControl *CreateControl(u32 id) override;
        void SetButtonHandlers(PushButton *pushButton) override;
        void HandleChange(u32 direction);
        u32 sheetIndex;
        void OnBackPress(u32 slotId);
        void SetPrevPage(PageId pageId);
        UpDownControl * upDownControls;

        PageId lastPage;
        MenuId lastMenu;

        u32 currentPageId;
        bool wasLanguageChanged;
        bool koreanChange;

        private:
        static void SaveSettings();
        void OnSaveButtonClick(PushButton * button, u32 hudSlotId);
        void OnUpDownClick(UpDownControl *upDownControl, u32 hudSlotId);
        void OnUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId);

        void OnTextChange(TextUpDownValueControl::TextControl *text, u32 optionId);
        void SaveAndQuit(PushButton *button);

        TextUpDownValueControl * textUpDown;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onUpDownClickHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onUpDownSelectHandler; 
        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onTextChangeHandler; 
        u32 scrollersCount;
        u8 optionsPerScroller[8];
        PtmfHolder_2A<Menu, void, PushButton *, u32> onRightButtonClickHandler;
        PtmfHolder_2A<Menu, void, PushButton *, u32> onLeftButtonClickHandler;
    };


} // namespace CosmosUI


#endif