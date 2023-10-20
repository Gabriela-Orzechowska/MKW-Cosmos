#ifndef _DX_SETTINGSSUBPAGE_
#define _DX_SETTINGSSUBPAGE_

#include <kamek.hpp>
#include <game/UI/MenuData/MenuData.hpp>
#include <game/UI/Ctrl/PushButton.hpp>
#include <game/UI/Page/Menu/Menu.hpp>
#include <game/UI/Page/Menu/VSSettings.hpp>
#include <game/UI/Ctrl/Menu/CtrlMenuText.hpp>
#include <UI/Settings/SettingsBasePage.hpp>
#include <main.hpp>

#define SUBPAGECOUNT 1

extern u32 VSSettings_sInstance;


namespace DXUI
{

    void Page_Refocus(Pages::Menu * page);

    class TextUpDownValueControlPlus : public TextUpDownValueControl
    {
        public:
        TextUpDownValueControlPlus(){};
        void SetMessage(u32 id) override;
        u32 id;
    };
    class SettingSubPage : public Pages::Menu
    {
        public:
        SettingSubPage(u32 selectorCount, const u8 optionCount[8], u32 sheetId);
        ~SettingSubPage() override;
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
        u32 sheetIndex;

        SettingsBasePage * basePage;


        private:
        static void SaveSettings();
        void OnBackPress(u32 slotId);
        void OnSaveButtonClick(PushButton * button, u32 hudSlotId);
        void OnUpDownClick(UpDownControl *upDownControl, u32 hudSlotId);
        void OnUpDownSelect(UpDownControl *upDownControl, u32 hudSlotId);

        void OnTextChange(TextUpDownValueControl::TextControl *text, u32 optionId);
        void SaveAndQuit(PushButton *button);

        UpDownControl * upDownControls;
        TextUpDownValueControlPlus * textUpDownPlus;
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onUpDownClickHandler; 
        PtmfHolder_2A<Page, void, UpDownControl*, u32> onUpDownSelectHandler; 
        PtmfHolder_2A<Page, void, TextUpDownValueControl::TextControl*, u32> onTextChangeHandler; 
        u32 scrollersCount;
        u8 optionsPerScroller[8];
        PtmfHolder_2A<Menu, void, PushButton *, u32> onRightButtonClickHandler;
        PtmfHolder_2A<Menu, void, PushButton *, u32> onLeftButtonClickHandler;

        UpDownControl * menuUpDownControls;
        TextUpDownValueControlPlus * menuTextUpDownPlus;

    };


} // namespace DXUI


#endif